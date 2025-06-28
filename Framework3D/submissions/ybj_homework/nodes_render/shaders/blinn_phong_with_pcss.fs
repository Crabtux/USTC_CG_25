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
            float w_kernal = ratio * (lights[i].radius * 0.1 * 0.5);

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

                float shadow_radius = (d_receiver - d_blocker) * lights[i].radius * 0.1 * 0.5 / d_blocker;

                if (shadow_radius < 0)          shadow_radius = 0;
                if (shadow_radius > 0.01)       shadow_radius = 0.01;

                vec2 shadow_topleft     = (projCoords.xy * 0.5 + 0.5 - shadow_radius);
                vec2 shadow_bottomright = (projCoords.xy * 0.5 + 0.5 + shadow_radius);

                for (float inner_x = max(shadow_topleft.x, 0); inner_x <= min(shadow_bottomright.x, 1); inner_x += texelSize.x) {
                    for (float inner_y = max(shadow_topleft.y, 0); inner_y <= min(shadow_bottomright.y, 1); inner_y += texelSize.y) {
                        shadow_number++;
                        float shadow_map_value = texture(shadow_maps, vec3(vec2(inner_x, inner_y), lights[i].shadow_map_id)).x;
                        float bias = max(0.05 * (1.0 - dot(normal, normalize(incident_light))), 0.005);
                        shadow += (current_shadow_value - bias > shadow_map_value) ? 1.0 : 0.0;
                    }
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