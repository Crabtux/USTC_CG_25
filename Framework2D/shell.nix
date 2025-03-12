{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  name = "USTC_CG_2025_HW01_devshell";

  packages = with pkgs; [
    clang-uml
    plantuml
  ];

  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
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
  ];

  # OpenGL runtime dependency is not written in the compiled executable
  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (with pkgs; [
    libGL
    libxkbcommon
    xorg.libX11
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
    xorg.libXext
    xorg.libXxf86vm
  ]);

  dlib_DIR = "/home/crabtux/code/cpp/USTC_CG_25/Framework2D/src/assignments/2_ImageWarping/_deps/dlib/lib64/cmake/dlib";
}

