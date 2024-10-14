# Godot LevelDB

A GDExtension providing a wrapper for the leveldb library. Compatible with Godot 4.1+.

# Credits
- The Godot team and all its contributors.
- Google and their developers that worked on [leveldb](https://github.com/google/leveldb/)
- [2Shady4U](https://github.com/2shady4u) from [https://github.com/2shady4u/godot-sqlite](https://github.com/2shady4u/godot-sqlite) for the GitHub Action scripts.

# Development

## Compiling the extension

To compile the extension run `scons platform=$TARGET`, where `$TARGET` is your target platform, for example `scons platform=linux` will compile the binaries for Linux systems.

## Documentation

To generate the documentation one must run the following command `godot --doctool ../ --gdextension-docs` inside the demo folder, after compiling the extension.