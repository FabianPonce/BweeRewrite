require "storage"

Quotes = {
	conn = nil,
	
	Initialize = function()
		conn = Storage.GetConnection()
		
		conn:exec([[
			CREATE TABLE IF NOT EXISTS quotes(id INTEGER PRIMARY KEY, addedBy VARCHAR(25) NOT NULL, message VARCHAR(255) NOT NULL)
		]])
	end,
	
	Cleanup = function()
	end,
	
	-- Adds a quote to the database and returns the quote's ID.
	AddQuote = function(handle, message)
		local parameter_names = { "addedBy", "message" }
		local stmt = conn:prepare(parameter_names, [[
			INSERT INTO quotes (addedBy,message) VALUES (:addedBy, :message);
		]])
		
		stmt:bind(handle, message)
		stmt:exec()
		
		local row = conn:first_row("SELECT MAX(id) AS id FROM quotes")
		return row.id
	end,
	
	-- Returns a random quote from the database.
	-- Return values: id, addedBy, message
	GetAnyQuote = function()
		local row = conn:first_row("SELECT id, addedBy, message FROM quotes ORDER BY RANDOM()")
		if row ~= nil then
			return row.id, row.addedBy, row.message
		else
			return nil
		end
	end,
	
	GetQuoteByID = function(id)
		local stmt = conn:prepare([[
			SELECT id, addedBy, message FROM quotes WHERE id = :id
		]])
		stmt:bind(id)
		local row = stmt:first_row()
		if row ~= nil then
			return row.id, row.addedBy, row.message
		else
			return nil
		end
	end,
	
	ResetQuotes = function()
		conn:exec([[
			DELETE FROM quotes
		]])
		
		-- Reset autoincrement on "quotes" table.
		conn:exec([[
			DELETE FROM sqlite_sequence WHERE name = 'quotes'
		]])
	end,
}

Quotes.Initialize()