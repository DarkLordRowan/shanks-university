# very sorry
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
            clang-tools
            poetry
            pkg-config
            mpfr
            gmp
            eigen
            mold
            ccache
            ninja
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
          ];
        };
      }
    );
}
