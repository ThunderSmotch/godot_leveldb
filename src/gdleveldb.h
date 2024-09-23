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
	Dictionary options;

	enum CompressionType {
		NO_COMPRESSION,
		SNAPPY_COMPRESSION
	};

protected:
	static void _bind_methods();

public:
	GDLevelDB();
	~GDLevelDB();

	bool open(String path);
	bool close();

	TypedArray<PackedByteArray> keys();
	PackedByteArray get(PackedByteArray key);
	// TODO put, delete
	void print(PackedByteArray bytes);
};

}

#endif