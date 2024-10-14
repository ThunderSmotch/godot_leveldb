#include "gdleveldb.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include "leveldb/db.h"

using namespace godot;

void GDLevelDB::_bind_methods() {
	ClassDB::bind_method(D_METHOD("open", "path"), &GDLevelDB::open);
	ClassDB::bind_method(D_METHOD("close"), &GDLevelDB::close);
	ClassDB::bind_method(D_METHOD("keys"), &GDLevelDB::keys);
	ClassDB::bind_method(D_METHOD("get", "key"), &GDLevelDB::get);
	ClassDB::bind_method(D_METHOD("put", "key", "value", "sync"), &GDLevelDB::put, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("delete", "key", "sync"), &GDLevelDB::_delete, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("print", "bytes"), &GDLevelDB::print);
}

GDLevelDB::GDLevelDB() {
	// Initialize any variables here.
	options["create_if_missing"] = false;
	options["error_if_exists"] = false;
	options["paranoid_checks"] = false;
}

GDLevelDB::~GDLevelDB() {
	// Add your cleanup here.
	if(db != nullptr)
	{
		delete db;
	}
}

bool GDLevelDB::open(String path) {
	
	if(db != nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot open a database with one already opened.");
		UtilityFunctions::printerr("[GDLevelDB] Please close the current database to open another.");
		return false;
	}
	
	// TODO implement customizable options
	UtilityFunctions::print("[GDLevelDB] Opening database at:", path);
	
	leveldb::Options options;
    options.create_if_missing = this->options["create_if_missing"];
	options.error_if_exists = this->options["error_if_exists"];
    options.paranoid_checks = this->options["paranoid_checks"];

	leveldb::Status status = leveldb::DB::Open(options, path.ascii().get_data(), &db);

	if (false == status.ok())
    {
		UtilityFunctions::printerr("[GDLevelDB] Unable to open database at", path);
        UtilityFunctions::printerr("[GDLevelDB] LevelDB error:", status.ToString().c_str());
        return false;
    }
	
	return true;
}

bool GDLevelDB::close()
{
	if(db == nullptr)
	{
		UtilityFunctions::printerr("Trying to close an opened LevelDB but none found.");
		return false;
	}

	delete db;
	db = nullptr;
	return true;
}

Array GDLevelDB::keys()
{
	Array keys;

	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot get keys from an unopened database!");
		return keys;
	}

	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

	// Grab end iterator to check if we reached the end otherwise we get corruption errors
	leveldb::Iterator* end = db->NewIterator(leveldb::ReadOptions());
	bool reached_end = false;
	end->SeekToLast();

    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
		size_t length = it->key().size();
		const char* data = it->key().data();

		PackedByteArray key_data;
		key_data.resize(length);
		std::copy(data, data + length, key_data.ptrw());
		keys.append(key_data);

		if(it->key().compare(end->key()) == 0)
		{
			reached_end = true;
			break;
		}
    }
    
    if (false == it->status().ok() && reached_end == false)
    {
		UtilityFunctions::printerr("[GDLevelDB] An error was found during the scan:");
		UtilityFunctions::printerr("[GDLevelDB] LevelDB Error:", it->status().ToString().c_str());
    }
    
    delete it;
	delete end;

	return keys;
}

PackedByteArray GDLevelDB::get(PackedByteArray key)
{
	PackedByteArray data;

	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot get value from an unopened database!");
		return data;
	}

	std::string value;
	leveldb::Status status = db->Get(leveldb::ReadOptions(), leveldb::Slice((const char *) key.ptr(), key.size()), &value);

	if (false == status.ok())
	{
		if (status.IsNotFound())
		{
			UtilityFunctions::printerr("[GDLevelDB] Could not get key:", key);
		}
		else
		{
			UtilityFunctions::printerr("[GDLevelDB] An error was found while getting value:");
			UtilityFunctions::printerr("[GDLevelDB] LevelDB Error:", status.ToString().c_str());
		}
		return data;
	}

	data.resize(value.length());
	std::copy(value.begin(), value.end(), data.ptrw());

	return data;
}

bool GDLevelDB::put(PackedByteArray key, PackedByteArray value, bool sync = false)
{
	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot put value into an unopened database!");
		return false;
	}

	auto key_slice = leveldb::Slice((const char *) key.ptr(), key.size());
	auto value_slice = leveldb::Slice((const char *) value.ptr(), value.size());
	
	auto options = leveldb::WriteOptions();
	options.sync = sync;

	// FIXME If Put is async won't value slice potentially point to dead memory?
	// Maybe this should be noted in the documentation.
	leveldb::Status status = db->Put(options, key_slice, value_slice);

	if (false == status.ok())
	{
		UtilityFunctions::printerr("[GDLevelDB] An error was found while putting value!");
		UtilityFunctions::printerr("[GDLevelDB] LevelDB Error:", status.ToString().c_str());
		return false;
	}

	return true;
}

bool GDLevelDB::_delete(PackedByteArray key, bool sync)
{
	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot delete key/value pair from an unopened database!");
		return false;
	}

	auto key_slice = leveldb::Slice((const char *) key.ptr(), key.size());
	auto options = leveldb::WriteOptions();
	options.sync = sync;

	leveldb::Status status = db->Delete(options, key_slice);

	if (false == status.ok())
	{
		UtilityFunctions::printerr("[GDLevelDB] An error was found while deleting key/value pair!");
		UtilityFunctions::printerr("[GDLevelDB] LevelDB Error:", status.ToString().c_str());
		return false;
	}

	return true;
}

// Attempts to print bytes assuming they are an ASCII string
// Will skip NULL characters (which are allowed in the middle of leveldb keys and values)
void GDLevelDB::print(PackedByteArray bytes)
{
	String string;
	for (auto byte: bytes)
	{
		if (byte != 0)
		{
			string += byte;
		}
	}
	UtilityFunctions::print(string);
}