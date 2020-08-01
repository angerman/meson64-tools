{ nixpkgs ? import <nixpkgs> {} }:
{
    meson64-tools = nixpkgs.stdenv.mkDerivation {
        name = "meson64-tools";
        src = ./.;
        makeFlags = [ "PREFIX=$(out)" ];
    };
}