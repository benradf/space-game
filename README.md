# space-game

A top-down multiplayer space shooter game inspired by
[Continuum](https://www.subspace-continuum.com/). Being far too ambitious in
scope I never finished it, but it was a good learning exercise nonetheless. It
is written in C++ and uses the [Ogre 3D](https://www.ogre3d.org/) graphics
engine. The models were created in [Blender](https://www.blender.org/).

![multiplayer.gif](assets/multiplayer.gif)

### Updated August 2022 to use Bazel and Nix

Recently I've been learning how to use [Bazel](https://github.com/bazelbuild/bazel)
and [Nix](https://github.com/tweag/rules_nixpkgs) to build projects so I decided to
try converting this one.

#### Building and running

You will need to [install nix](https://nixos.org/download.html) if you do not already have it.

```
# First start the server
nix-shell --run 'bazel run //common:run-server'

# Now connect to it with a client
nix-shell --run 'bazel run //common:run-client'
```

You can run multiple clients on the same machine and they will all connect to
the same server on localhost. It should work across a network too but you will
need to pass the IP address of the server when you run the client, e.g.

```
SERVER_IP=192.168.0.10 nix-shell --run 'bazel run //common:run-client'
```

Please do not expose the server to an untrusted network because it is not secure.

#### Controls

* *Left/Right*: rotate ship
* *Up*: engage thrusters to move forward
* *Ctrl*: hold together with *Up* to boost speed
