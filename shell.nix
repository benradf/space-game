with import <nixpkgs> {
  config = { };
  overlays = [ ];
};

mkShell {
  name = "rules_nixpkgs_shell";
  packages = [ bazel_5 bazel-buildtools cacert gcc nix git ];
}
