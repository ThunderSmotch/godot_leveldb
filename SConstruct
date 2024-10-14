#!/usr/bin/env python

target_path = ARGUMENTS.pop("target_path", "demo/bin/")
target_name = ARGUMENTS.pop("target_name", "libgdleveldb")
target = "{}{}".format(
    target_path, target_name
)

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
env.Append(CPPPATH=["src/", "src/leveldb/include/", "src/leveldb/"])
sources = [Glob("src/leveldb/*/*.cc", 
                exclude=['src/leveldb/*/*_test.cc', 'src/leveldb/benchmarks/*.cc',
                        'src/leveldb/util/env_*.cc', 'src/leveldb/util/testutil.cc']), Glob("src/*.cpp")]

# Godot Extension Documentation
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# leveldb defines and sources according to platform 
if env["platform"] == "windows":
    env.Append(CPPDEFINES=['LEVELDB_PLATFORM_WINDOWS'])
    sources.append(Glob("src/leveldb/util/env_windows.cc"))
else:
    env.Append(CPPDEFINES=['LEVELDB_PLATFORM_POSIX'])
    sources.append(Glob("src/leveldb/util/env_posix.cc"))


if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{}.{}.{}.framework/{}.{}.{}".format(
            target, env["platform"], env["target"], target_name, env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}{}{}".format(target, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
