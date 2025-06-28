#version 430 core

// Define a uniform struct for lights
struct Light {
    // The matrices are used for shadow mapping. You need to fill it according to how we are filling it when building the normal maps (node_render_shadow_mapping.cpp). 
    // Now, they are filled with identity matrix. You need to modify C++ code innode_render_deferred_lighting.cpp.
    // Position and color are filled.
    mat4 light_projection;
    mat4 light_view;
    vec3 position;
    float radius;
    vec3 color; // Just use the same diffuse and specular color.
    int shadow_map_id;
};

layout(binding = 0) buffer lightsBuffer {
    Light lights[4];
};

uniform vec2 iResolution;

uniform sampler2D diffuseColorSampler;
uniform sampler2D normalMapSampler; // You should apply normal mapping in rasterize_impl.fs
uniform sampler2D metallicRoughnessSampler;
uniform sampler2DArray shadow_maps;
uniform sampler2D position;

uniform vec3 camPos;

uniform int light_count;

layout(location = 0) out vec4 Color;

#define POISSON_SAMPLE_SIZE (64)
#define POISSON_SAMPLE_NUM  (16)

// predefined poisson sampling points
const vec2 poisson[POISSON_SAMPLE_SIZE] = vec2[](
	vec2(-0.884081, 0.124488),
	vec2(-0.714377, 0.027940),
	vec2(-0.747945, 0.227922),
	vec2(-0.939609, 0.243634),
	vec2(-0.985465, 0.045534),
	vec2(-0.861367, -0.136222),
	vec2(-0.881934, 0.396908),
	vec2(-0.466938, 0.014526),
	vec2(-0.558207, 0.212662),
	vec2(-0.578447, -0.095822),
	vec2(-0.740266, -0.095631),
	vec2(-0.751681, 0.472604),
	vec2(-0.553147, -0.243177),
	vec2(-0.674762, -0.330730),
	vec2(-0.402765, -0.122087),
	vec2(-0.319776, -0.312166),
	vec2(-0.413923, -0.439757),
	vec2(-0.979153, -0.201245),
	vec2(-0.865579, -0.288695),
	vec2(-0.243704, -0.186378),
	vec2(-0.294920, -0.055748),
	vec2(-0.604452, -0.544251),
	vec2(-0.418056, -0.587679),
	vec2(-0.549156, -0.415877),
	vec2(-0.238080, -0.611761),
	vec2(-0.267004, -0.459702),
	vec2(-0.100006, -0.229116),
	vec2(-0.101928, -0.380382),
	vec2(-0.681467, -0.700773),
	vec2(-0.763488, -0.543386),
	vec2(-0.549030, -0.750749),
	vec2(-0.809045, -0.408738),
	vec2(-0.388134, -0.773448),
	vec2(-0.429392, -0.894892),
	vec2(-0.131597, 0.065058),
	vec2(-0.275002, 0.102922),
	vec2(-0.106117, -0.068327),
	vec2(-0.294586, -0.891515),
	vec2(-0.629418, 0.379387),
	vec2(-0.407257, 0.339748),
	vec2(0.071650, -0.384284),
	vec2(0.022018, -0.263793),
	vec2(0.003879, -0.136073),
	vec2(-0.137533, -0.767844),
	vec2(-0.050874, -0.906068),
	vec2(0.114133, -0.070053),
	vec2(0.163314, -0.217231),
	vec2(-0.100262, -0.587992),
	vec2(-0.004942, 0.125368),
	vec2(0.035302, -0.619310),
	vec2(0.195646, -0.459022),
	vec2(0.303969, -0.346362),
	vec2(-0.678118, 0.685099),
	vec2(-0.628418, 0.507978),
	vec2(-0.508473, 0.458753),
	vec2(0.032134, -0.782030),
	vec2(0.122595, 0.280353),
	vec2(-0.043643, 0.312119),
	vec2(0.132993, 0.085170),
	vec2(-0.192106, 0.285848),
	vec2(0.183621, -0.713242),
	vec2(0.265220, -0.596716),
	vec2(-0.009628, -0.483058),
	vec2(-0.018516, 0.435703)
);

vec2 samplePoisson(uint index) {
	return poisson[index % POISSON_SAMPLE_SIZE];
}

int MIN = 0;
int MAX = POISSON_SAMPLE_SIZE;

int xorshift(in int value) {
    // Xorshift*32
    // Based on George Marsaglia's work: http://www.jstatsoft.org/v08/i14/paper
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    return value;
}

int nextInt(inout int seed) {
    seed = xorshift(seed);
    return seed;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution;

    vec3 pos = texture(position, uv).xyz;
    vec3 normal = texture(normalMapSampler, uv).xyz;
    vec3 diffuseColor = texture(diffuseColorSampler, uv).xyz;

    vec4 metalnessRoughness = texture(metallicRoughnessSampler, uv);
    float metal = metalnessRoughness.x;
    float roughness = metalnessRoughness.y;

    Color = vec4(0, 0, 0, 1);

    for(int i = 0; i < light_count; i++) {
        vec4 cur_color   = vec4(0, 0, 0, 0);
        float k_specular = metal * 0.8;
        float k_diffuse  = 1 - k_specular;

        float alpha      = (1 - roughness) * 3;
        float k_soft     = 0.08;

        // HW6_TODO: first comment the line above ("Color +=..."). That's for quick Visualization.
        // You should first do the Blinn Phong shading here. You can use roughness to modify alpha. Or you can pass in an alpha value through the uniform above.
        vec3 light_color    = lights[i].color;
        vec3 light_position = lights[i].position;

        // 漫反射
        float cos_diffuse = dot(light_position, normal) / (length(light_position) * length(normal));
        vec3  i_diffuse   = light_color * diffuseColor * k_diffuse * max(cos_diffuse, 0.0);
        cur_color += vec4(i_diffuse.x, i_diffuse.y, i_diffuse.z, 0);

        // 镜面反射
        vec3  incident_light  = light_position - pos;
        vec3  view            = pos - camPos;
        vec3  specular_vector = reflect(incident_light, normalize(normal));
        float cos_specular    = dot(specular_vector, view) / (length(specular_vector) * length(view));
        vec3  i_specular      = light_color * k_specular * max(pow(cos_specular, alpha), 0.0);
        cur_color += vec4(i_specular.x, i_specular.y, i_specular.z, 0);

        // After finishing Blinn Phong shading, you can do shadow mapping with the help of the provided shadow_map_value. You will need to refer to the node, node_render_shadow_mapping.cpp, for the light matrices definition. Then you need to fill the mat4 light_projection; mat4 light_view; with similar approach that we fill position and color.
        // For shadow mapping, as is discussed in the course, you should compare the value "position depth from the light's view" against the "blocking object's depth.", then you can decide whether it's shadowed.

        // PCSS is also applied here.
        int   shadow_number = 0;
        float shadow        = 0.0;

        if (pos.z < light_position.z - 1) {
            // Get Lookup range for Step 1
            vec4 fragPosLightSpace = lights[i].light_projection * lights[i].light_view * vec4(pos, 1.0);
            vec3 projCoords        = fragPosLightSpace.xyz / fragPosLightSpace.w;
            vec3 texelSize         = 1.0 / textureSize(shadow_maps, i);

            float d_receiver           = projCoords.z;
            float current_shadow_value = projCoords.z;

            // 1 是近平面的高度
            float ratio    = (light_position.z - 1 - pos.z) / (light_position.z - pos.z);
            float w_kernal = ratio * (lights[i].radius * k_soft * 0.5);

            // 不加限制器会死人的！
            if (w_kernal < 0)           w_kernal = 0;
            if (w_kernal > 0.01)        w_kernal = 0.01;

            vec2 kernel_topleft     = (projCoords.xy * 0.5 + 0.5 - w_kernal);
            vec2 kernel_bottomright = (projCoords.xy * 0.5 + 0.5 + w_kernal);

            bool  is_crossed   = true;
            int   block_number = 0;
            float d_blocker    = 0.0;
            for (float inner_x = max(kernel_topleft.x, 0); inner_x <= min(kernel_bottomright.x, 1); inner_x += texelSize.x) {
                for (float inner_y = max(kernel_topleft.y, 0); inner_y <= min(kernel_bottomright.y, 1); inner_y += texelSize.y) {
                    is_crossed = false;
                    float shadow_map_value = texture(shadow_maps, vec3(inner_x, inner_y, lights[i].shadow_map_id)).x;
                    float bias = max(0.05 * (1.0 - dot(normal, normalize(incident_light))), 0.005);
                    if (current_shadow_value - bias > shadow_map_value) {
                        block_number++;
                        d_blocker += shadow_map_value;
                    }
                }
            }

            if (block_number == 0) {
                shadow = is_crossed ? 1 : 0;
                shadow_number = 0;
            }
            else {
                d_blocker /= block_number;

                float shadow_radius = (d_receiver - d_blocker) * lights[i].radius * k_soft * 0.5 / d_blocker;

                if (shadow_radius < 0)          shadow_radius = 0;
                if (shadow_radius > 0.01)       shadow_radius = 0.01;

                vec2 shadow_topleft     = (projCoords.xy * 0.5 + 0.5 - shadow_radius);
                vec2 shadow_bottomright = (projCoords.xy * 0.5 + 0.5 + shadow_radius);

                int rngSeed = int(gl_FragCoord.x) + int(gl_FragCoord.y) * int(iResolution.x)
                            + int(camPos.x * 1024) + int(camPos.y * 1024) * 2048;
                for (int t = 0; t < POISSON_SAMPLE_NUM * (shadow_radius / 0.001); t++)
                {
                    // Poisson Disc Sampling
                    rngSeed = nextInt(rngSeed);
                    vec2 inner_xy = samplePoisson(rngSeed) * shadow_radius + projCoords.xy * 0.5 + 0.5;
                    if (inner_xy.x < 0 || inner_xy.x > 1 || inner_xy.y < 0 || inner_xy.y > 1)
                        continue;

                    float shadow_map_value = texture(shadow_maps, vec3(inner_xy, lights[i].shadow_map_id)).x;
                    float bias = max(0.05 * (1.0 - dot(normal, normalize(incident_light))), 0.005);

                    shadow += (current_shadow_value - bias > shadow_map_value) ? 1.0 : 0.0;
                    shadow_number++;
                }

                // 全在外面
                if (shadow_number == 0)
                    shadow = 1;
            }  
        }
        else {
            shadow = 1;
        }

        if (shadow_number == 0)
            shadow_number = 1;

        // 应用阴影
        cur_color *= (1.0 - shadow / shadow_number);

        // 环境光
        Color += cur_color + vec4(0.05 * light_color, 1);

        // 底色，看起来效果还不错
        Color += vec4(0.2 * diffuseColor, 1);
    }
}