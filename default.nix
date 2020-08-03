{ nixpkgs ? import <nixpkgs> {} }:
{
    meson64-tools = nixpkgs.stdenv.mkDerivation {

        nativeBuildInputs = [ nixpkgs.python2 nixpkgs.python3 ];

        name = "meson64-tools";
        src = ./.;

        preBuild = ''
            patchShebangs .
            patchShebangs ./mbedtls/scripts/generate_psa_constants.py
        '';

        makeFlags = [ "PREFIX=$(out)" ];
    };
}