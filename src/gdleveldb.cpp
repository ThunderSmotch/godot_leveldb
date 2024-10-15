#include "gdleveldb.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include "leveldb/db.h"

using namespace godot;


inline leveldb::Slice slice_from_bytes(PackedByteArray bytes)
{
	return leveldb::Slice((const char *) bytes.ptr(), bytes.size());
}

void GDLevelDB::_bind_methods() {
	Dictionary options = Dictionary();
	options["create_if_missing"] = false;
	options["error_if_exists"] = false;
	options["paranoid_checks"] = false;

	Dictionary write_options = Dictionary();
	write_options["sync"] = false;

	Dictionary read_options = Dictionary();
	read_options["verify_checksums"] = false;
	read_options["fill_cache"] = true;

	ClassDB::bind_method(D_METHOD("open", "path", "options"), &GDLevelDB::open, DEFVAL(options));
	ClassDB::bind_method(D_METHOD("close"), &GDLevelDB::close);
	ClassDB::bind_method(D_METHOD("keys"), &GDLevelDB::keys);
	ClassDB::bind_method(D_METHOD("get", "key", "options"), &GDLevelDB::get, DEFVAL(read_options));
	ClassDB::bind_method(D_METHOD("put", "key", "value", "options"), &GDLevelDB::put, DEFVAL(write_options));
	ClassDB::bind_method(D_METHOD("delete", "key", "options"), &GDLevelDB::_delete, DEFVAL(write_options));
	ClassDB::bind_method(D_METHOD("print", "bytes"), &GDLevelDB::print);
}

GDLevelDB::GDLevelDB() {
	// Initialize any variables here.
}

GDLevelDB::~GDLevelDB() {
	// Add your cleanup here.
	if(db != nullptr)
	{
		delete db;
	}
}

bool GDLevelDB::open(String path, Dictionary options) {
	
	if(db != nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot open a database with one already opened.");
		UtilityFunctions::printerr("[GDLevelDB] Please close the current database to open another.");
		return false;
	}
	
	UtilityFunctions::print("[GDLevelDB] Opening database at:", path);
	
	leveldb::Options db_options;
	db_options.create_if_missing = options.get("create_if_missing", false);
	db_options.error_if_exists = options.get("error_if_exists", false);
    db_options.paranoid_checks = options.get("paranoid_checks", false);

	leveldb::Status status = leveldb::DB::Open(db_options, path.ascii().get_data(), &db);

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

PackedByteArray GDLevelDB::get(PackedByteArray key, Dictionary options)
{
	PackedByteArray data;

	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot get value from an unopened database!");
		return data;
	}

	leveldb::ReadOptions read_options;
	read_options.verify_checksums = options.get("verify_checksums", false);
	read_options.fill_cache = options.get("fill_cache", true);

	std::string value;
	leveldb::Status status = db->Get(read_options, slice_from_bytes(key), &value);

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

bool GDLevelDB::put(PackedByteArray key, PackedByteArray value, Dictionary options)
{
	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot put value into an unopened database!");
		return false;
	}
	
	leveldb::WriteOptions write_options;
	write_options.sync = options.get("sync", false);

	// FIXME If Put is async won't value slice potentially point to dead memory?
	// Maybe this should be noted in the documentation.
	leveldb::Status status = db->Put(write_options, slice_from_bytes(key), slice_from_bytes(value));

	if (false == status.ok())
	{
		UtilityFunctions::printerr("[GDLevelDB] An error was found while putting value!");
		UtilityFunctions::printerr("[GDLevelDB] LevelDB Error:", status.ToString().c_str());
		return false;
	}

	return true;
}

bool GDLevelDB::_delete(PackedByteArray key, Dictionary options)
{
	if(db == nullptr)
	{
		UtilityFunctions::printerr("[GDLevelDB] Cannot delete key/value pair from an unopened database!");
		return false;
	}
	
	leveldb::WriteOptions write_options;
	write_options.sync = options.get("sync", false);

	leveldb::Status status = db->Delete(write_options, slice_from_bytes(key));

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