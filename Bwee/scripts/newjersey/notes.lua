require "storage"

Notes = {
	conn = nil,
	
	Initialize = function()
		conn = Storage.GetConnection()
		
		local res = conn:exec([[
			CREATE TABLE IF NOT EXISTS notes (id INTEGER PRIMARY KEY, nickTo VARCHAR(25) NOT NULL, nickFrom VARCHAR(25) NOT NULL, message VARCHAR(255) NOT NULL)
		]])
		assert(res)
	end,
	
	Cleanup = function()
	end,
	
	AddNote = function(to, from, message)
		local parameter_names = { "to", "from", "message" }
		local stmt = conn:prepare(parameter_names, [[
			INSERT INTO notes (nickTo,nickFrom,message) VALUES (lower(:to), :from, :message);
		]])
		stmt:bind(to, from, message)
		stmt:exec()
	end,
	
	-- Returns a single note and who it was from, then removes it from the database.
	HasNote = function(nick)
		local stmt = conn:prepare([[
			SELECT id, nickTo, nickFrom, message FROM notes WHERE nickTo = lower(:nick)
		]])
		stmt:bind(nick)
		local row = stmt:first_row()
		if row ~= nil then
			stmt = conn:prepare([[
				DELETE FROM notes WHERE id = :id
			]])
			stmt:bind(row.id)
			stmt:exec()
			
			return row.nickFrom, row.message
		else
			return nil
		end
	end,
	
	ResetNotes = function()
		conn:exec([[
			DELETE FROM notes
		]])
		
		-- Reset autoincrement on "queued_notes" table.
		conn:exec([[
			DELETE FROM notes WHERE name = 'queued_notes'
		]])
	end,
	
	ProcessNotes = function(context, handle)
		local noteFrom, noteMessage = Notes.HasNote(handle)
		while noteFrom ~= nil do
			SendMessage(context, string.format(
			[[%s, you have a note from %s: %s]], handle, noteFrom, noteMessage))
	
			noteFrom, noteMessage = Notes.HasNote(handle)
		end
	end,
}

Notes.Initialize()