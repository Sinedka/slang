{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs = { self, nixpkgs }:
  let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = with pkgs; [
      ];
      # set the environment variables that Qt apps expect
      shellHook = ''
        export CC=/etc/profiles/per-user/sinedka/bin/clang
        export CXX=/etc/profiles/per-user/sinedka/bin/clang++
      '';
    };
  };
}
