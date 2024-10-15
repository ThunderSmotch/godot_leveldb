#ifndef GDLEVELDB_H
#define GDLEVELDB_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#include "leveldb/db.h"

namespace godot {

class GDLevelDB : public RefCounted {
	GDCLASS(GDLevelDB, RefCounted)

private:
	leveldb::DB* db = nullptr;

protected:
	static void _bind_methods();

public:
	GDLevelDB();
	~GDLevelDB();

	bool open(String path, Dictionary options);
	bool close();

	// TODO: Swap to TypedArray<PackedByteArray> when godot-cpp updates
	Array keys();

	PackedByteArray get(PackedByteArray key, Dictionary options);
	bool put(PackedByteArray key, PackedByteArray value, Dictionary options);
	bool _delete(PackedByteArray key, Dictionary options);

	void print(PackedByteArray bytes);
};

}

#endif