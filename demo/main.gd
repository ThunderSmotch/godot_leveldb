extends Node

var db = GDLevelDB.new()

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("Test LevelDB")
	db.print()
	db.open("./leveldb")
	var keys = db.keys()
	#print(db.get(keys[3]).get_string_from_ascii())
	var key: PackedByteArray = keys[3]
	print(key.get_string_from_utf8())

	
	db.print_key(keys[3])
	
	
