require "storage"
Auth_AuthenticatedUsers = { }
Auth = {
	FLAG_SUPERADMIN = "z",
	conn = nil,
	
	Initialize = function()
		conn = Storage.GetConnection()
		
		local res = conn:exec([[
			CREATE TABLE IF NOT EXISTS auth (nick VARCHAR(25) PRIMARY KEY NOT NULL, passcode VARCHAR(255) NOT NULL, flags VARCHAR(255), lastUserHost VARCHAR(255))
		]])
		assert(res)
		
		local stmt = conn:prepare([[
			SELECT nick, passcode, lastUserHost FROM auth WHERE lastUserHost <> ""
		]])
		for row in stmt:rows() do
			Auth_AuthenticatedUsers[row.nick] = row.lastUserHost
		end
	end,
	
	Cleanup = function()
	end,
	
	AddUser = function(nick, passcode, flags)
		local stmt = conn:prepare([[
			INSERT INTO auth VALUES (:nick, :passcode, :flags)
		]])
		stmt:bind(nick, passcode, flags)
		stmt:exec()
	end,
	
	AddFlag = function(nick, flag)
		if Auth.HasFlag(nick, flag) == true then
			return
		end
		
		local stmt = conn:prepare([[
			UPDATE auth SET flags = flags || :flag WHERE nick = :nick
		]])
		
		stmt:bind(flag, nick)
		stmt:exec()
	end,
	
	HasFlag = function(nick, flag)
		local stmt = conn:prepare([[
			SELECT flags FROM auth WHERE nick = lower(:nick)
		]])
		stmt:bind(nick)
		local row = stmt:first_row()
		if row == nil then
			return false
		end
		
		if row.flags:find(flag) ~= nil or row.flags:find("z") then
			return true
		else
			return false
		end
	end,
	
	IsRegistered = function(nick)
		local stmt = conn:prepare([[
			SELECT COUNT(*) AS count FROM auth WHERE nick = lower(:nick)
		]])
		stmt:bind(nick)
		local row = stmt:first_row()
		if row == nil then
			return false
		end
		
		if row.count ~= 0 then
			return true
		else
			return false
		end
	end,
	
	GetFlags = function(nick)
		local stmt = conn:prepare([[
			SELECT flags FROM auth WHERE nick = lower(:nick)
		]])
		stmt:bind(nick)
		local row = stmt:first_row()
		if row == nil then
			return ""
		end
		
		return row.flags
	end,
	
	Login = function(nick, userHost, passcode)
		local stmt = conn:prepare([[
			SELECT passcode FROM auth WHERE nick = lower(:nick)
		]])
		stmt:bind(nick)
		
		local row = stmt:first_row()
		if row ~= nil then
			if row.passcode == passcode then
				Auth_AuthenticatedUsers[nick:lower()] = userHost
				local stmt = conn:prepare([[UPDATE auth SET lastUserHost = :lastUserHost WHERE nick = lower(:nick)]])
				stmt:bind(userHost, nick)
				stmt:exec()
				return true
			else
				return false
			end
		else
			return false
		end
	end,
	
	Logout = function(nick)
		local stmt = conn:prepare([[UPDATE auth SET lastUserHost = "" WHERE nick = lower(:nick)]])
		stmt:bind(nick)
		stmt:exec()
		Auth_AuthenticatedUsers[nick:lower()] = nil
	end,
	
	IsAuthed = function(nick, userHost)
		if Auth_AuthenticatedUsers[nick:lower()] == userHost then
			return true
		end
		
		return false
	end,
	
	IsAuthedWithFlags = function(nick, userHost, reqFlags)
		if Auth.IsAuthed(nick, userHost) and Auth.HasFlag(nick, reqFlags) then
			return true
		else
			return false
		end
	end,
}

Auth.Initialize()