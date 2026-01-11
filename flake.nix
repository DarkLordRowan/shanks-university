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

        devShells.python = pkgs.mkShell {
          packages = with pkgs; [
            (python3.withPackages (ps: with ps; [
              numpy
              pandas
              pyarrow
              polars
              pyqt6
              pyqtgraph
              mypy
            ]))
          ];
        };
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
            clang-tools
            poetry
            pkg-config
            mpfr
            gmp
            eigen
            (python3.withPackages (
              ps: with ps; [
                pybind11-stubgen
                polars
                pyarrow
                pyqt6
                pyqtgraph
                numpy
                pandas
              ]
            ))
          ];
        };
      }
    );
}
