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

    zlib
    tbb
    shader-slang

    vulkan-headers
    vulkan-loader
    vulkan-tools
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

  PXR_PLUGINPATH_NAME = "/home/crabtux/code/cpp/USTC_CG_25/Framework3D/Framework3D/SDK/OpenUSD/Debug/lib/usd";

  # shellHook = ''
  #   echo ${pkgs.shader-slang.dev}
  #   export CMAKE_INCLUDE_PATH=$CMAKE_INCLUDE_PATH:${pkgs.python3}/include/python3.12
  # '';
}

