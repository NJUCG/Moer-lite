# SPDX-FileCopyrightText: 2021-2025 Akira Komamura
# SPDX-License-Identifier: Unlicense
{
  inputs = {
    # nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    systems.url = "github:nix-systems/default";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      systems,
    }:
    flake-utils.lib.eachSystem (import systems) (
      system:
      let
        pkgs = import nixpkgs {
          system = if (builtins.match ".*darwin.*" system != null)
            then "x86_64-darwin"
            else system;
          # inherit system;
        };
      in
      {
        devShells.default = pkgs.stdenvNoCC.mkDerivation {
          name = "More-lite dev environment";
          buildInputs = with pkgs; [
            git
            cmake
            clang_19
            llvmPackages_19.clang-tools
            llvmPackages_19.openmp
          ];
        };
      }
    );
}
