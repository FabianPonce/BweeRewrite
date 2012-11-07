require "sqlite3"

Storage = {
	db = nil,
	
	Initialize = function()
		db = sqlite3.open("bwee.sqlite3")
	end,
	
	GetConnection = function()
		return db
	end,
	
	Cleanup = function()
		assert(db:close())
	end,
}

Storage.Initialize()