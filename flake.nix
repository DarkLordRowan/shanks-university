# very sorry
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    fenix = {
      url = "github:nix-community/fenix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    { nixpkgs, flake-utils, fenix, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            ccache
            clang-tools
            cmake
            eigen
            gmp
            gsl
            mold
            mpfr
            ninja
            pkg-config
            poetry
            sqlite
            (python3.withPackages (
              ps: with ps; [
                pybind11-stubgen
                polars
                pyarrow
                pyqt6
                pyqtgraph
                numpy
                pandas
                mypy
              ]
            ))
            fenix.packages.${system}.latest.toolchain
          ];
          LD_LIBRARY_PATH = with pkgs; lib.makeLibraryPath [ libGL wayland libxkbcommon vulkan-loader ];
        };
      }
    );
}
