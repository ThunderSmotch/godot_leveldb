# Godot LevelDB

A GDExtension providing a wrapper for the LevelDB library. Compatible with Godot 4.1+.

# Credits
- The Godot team and all its contributors.
- Google and the developers that worked on [LevelDB](https://github.com/google/leveldb/)
- [2Shady4U](https://github.com/2shady4u) from [https://github.com/2shady4u/godot-sqlite](https://github.com/2shady4u/godot-sqlite) for the GitHub Action scripts.

# Features for future versions
- [ ] Snapshots
- [ ] Batch operations

# Development

## Compiling the extension

To compile the extension run `scons platform=$PLATFORM target=$TARGET`, where `$PLATFORM` is your target platform and `$TARGET` your target build (debug or release), for example `scons platform=linux target=template_debug` will compile the debug binaries for Linux systems.

## Documentation

To generate the documentation one must run the following command `godot --doctool ../ --gdextension-docs` inside the demo folder, after compiling the extension. When editing the docs, you may need to delete the library binaries and recompile them for the documentation to update in the Godot editor.