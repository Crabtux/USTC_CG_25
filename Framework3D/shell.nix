{ pkgs ? import <nixpkgs> {} }:

let
  # Matches the pyside6-uic implementation
  # https://code.qt.io/cgit/pyside/pyside-setup.git/tree/sources/pyside-tools/pyside_tool.py?id=e501cad66146a49c7a259579c7bb94bc93a67a08#n82
  pyside-tools-uic = pkgs.writeShellScriptBin "pyside6-uic" ''
    exec ${pkgs.qt6.qtbase}/libexec/uic -g python "$@"
  '';
in
pkgs.mkShell {
  name = "USTC_CG_2025_Framework3D_devshell";

  packages = with pkgs; [
    clang-uml
    plantuml

    # 这就是我们双显卡玩家的福报啊！
    vkdevicechooser

    mesa-demos
  ];

  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    wayland-scanner
    python3
    ninja
    pyside-tools-uic
    (python3.withPackages (ps: with ps; [
      requests
      tqdm
      pyopengl
      pyside6
      numpy
      jinja2
    ]))
  ];

  buildInputs = with pkgs; [
    libGL
    libxkbcommon
    xorg.libX11
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
    xorg.libXext
    xorg.libXxf86vm
    xorg.libXt
    libunwind

    zlib
    tbb
    shader-slang

    # For Vulkan shaderc
    shaderc.static
    spirv-tools

    # 都没啥用，SPIRV-Reflect 和 VMA 都是手加的
    shaderc
    # spirv-headers
    # glslang

    vulkan-headers
    vulkan-loader
    vulkan-tools
    vulkan-validation-layers
    vulkan-memory-allocator
    vulkan-utility-libraries
  ];

  # OpenGL runtime dependency is not written in the compiled executable
  LD_LIBRARY_PATH =
    pkgs.lib.makeLibraryPath (with pkgs; [
      libGL
      libxkbcommon
      xorg.libX11
      xorg.libXrandr
      xorg.libXinerama
      xorg.libXcursor
      xorg.libXi
      xorg.libXext
      xorg.libXxf86vm
      xorg.libXt

      zlib
      tbb
      shader-slang

      vulkan-headers
      vulkan-loader
      vulkan-tools
    ]);

  dlib_DIR = "/home/crabtux/code/cpp/USTC_CG_25/Framework2D/src/assignments/2_ImageWarping/_deps/dlib/lib64/cmake/dlib";

  PXR_PLUGINPATH_NAME = "/home/crabtux/code/cpp/USTC_CG_25/Framework3D/Framework3D/Binaries/Debug/usd/hd_USTC_CG/resources:/home/crabtux/code/cpp/USTC_CG_25/Framework3D/Framework3D/Binaries/Debug/usd/hd_USTC_CG_GL/resources:/home/crabtux/code/cpp/USTC_CG_25/Framework3D/Framework3D/Binaries/Debug/usd/hd_USTC_CG_Embree/resources";

  VULKAN_SDK = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
  # VULKAN_SDK = "${pkgs.vulkan-headers}";

  shaderc_combined_PATH = "${pkgs.shaderc.static}";

  # 这救世我们双显卡玩家的福报啊！（二度）
  ENABLE_DEVICE_CHOOSER_LAYER = 1;
  VULKAN_DEVICE_INDEX = 0;

  shellHook = ''
    echo ${pkgs.shader-slang}
  '';
}

