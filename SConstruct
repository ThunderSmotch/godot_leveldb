#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# Should change the env_windows.cc to other excludes depending on target arch...

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", "src/leveldb/include/", "src/leveldb/"], CPPDEFINES=['LEVELDB_PLATFORM_POSIX'])
sources = [Glob("src/leveldb/*/*.cc", exclude=['src/leveldb/*/*_test.cc', 'src/leveldb/benchmarks/*.cc', 
                                               'src/leveldb/util/env_windows.cc', 'src/leveldb/util/testutil.cc']), Glob("src/*.cpp")]

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libgdleveldb.{}.{}.framework/libgdleveldb.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/libgdleveldb{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
