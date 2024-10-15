extends Node

var db = GDLevelDB.new()

func _ready() -> void:
	# Opening database (creating one if it does not exist)
	db.open("./demo_db", {"create_if_missing": true})
	
	# Saving a string and an integer
	var a_key: PackedByteArray = "key1".to_ascii_buffer()
	var other_key: PackedByteArray = "key2".to_ascii_buffer()
	
	var string: String = "value"
	
	var i: int = 1000
	var bytes: PackedByteArray
	bytes.resize(8) # 8 bytes = 64 bits, enough for an int
	bytes.encode_s64(0, i)
	
	db.put(a_key, string.to_ascii_buffer())
	db.put(other_key, bytes)
	
	# Print all the keys in this database assuming they were strings
	var keys = db.keys()
	for key in keys:
		db.print(key)
	
	# Print the string
	db.print(db.get(a_key))
	
	# Grab signed 64-bit int value
	print(db.get(other_key).decode_s64(0))
	
	# Deleting a key/value pair
	db.delete(a_key)
	
	# Close the database
	db.close()
