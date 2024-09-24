#ifndef GDLEVELDB_REGISTER_TYPES_H
#define GDLEVELDB_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_leveldb_module(ModuleInitializationLevel p_level);
void uninitialize_leveldb_module(ModuleInitializationLevel p_level);

#endif // GDLEVELDB_REGISTER_TYPES_H