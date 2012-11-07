require "auth"

Commands_CmdTable = { }
Commands_ReqAccess = { }
Commands_HelpTable = { }

Commands = {
	RegisterCommand = function(trigger, func, helpText, authFlag)
		Commands_CmdTable[trigger] = func
		Commands_HelpTable[trigger] = helpText
		if authFlag ~= nil then
			Commands_ReqAccess[trigger] = authFlag
		end
	end,
	
	TriggerCommand = function(trigger, sender, channel, params)
		local func = Commands_CmdTable[trigger]
		if func ~= nil then
			if Commands_ReqAccess[trigger] ~= nil and not Auth.IsAuthedWithFlags(sender, GetSenderUserHostString(), Commands_ReqAccess[trigger]) then
				SendMessage(channel, "You are not authorized to use that command.")
				return true
			end
			func(sender, channel, params)
			return true
		else
			Commands.HandleNoSuchCommand(sender, channel, params)
			return false
		end
	end,
	
	HandleNoSuchCommand = function(sender, channel, params)
		SendMessage(channel, string.format([[No command exists with that name, %s. Try using the help command if you're confused.]], sender))
	end,
	
	HandleHelpCommand = function(sender, channel, params)
		local helpText = Commands_HelpTable[params]
		if helpText == nil then 
			if params ~= nil and params ~= "" then
				Commands.HandleNoSuchCommand(sender, channel, params)
				return 
			else
				helpText = Commands_HelpTable["help"]
			end
		end
		
		assert(helpText)
		SendMessage(channel, helpText)
		
		if helpText == Commands_HelpTable["help"] then
			local availableCommands = "Available Commands: "
			for k,v in pairs(Commands_HelpTable) do
				availableCommands = availableCommands .. k .. " "
			end
			SendMessage(channel, availableCommands)
		end
	end,
	
	HandleLoginCommand = function(sender, channel, params)
		if channel:sub(1,1) == "#" then
			SendMessage(channel, "Please only use the login command via private message.")
			return
		end
		
		if Auth.IsAuthed(sender, GetSenderUserHostString()) then
			SendMessage(sender, "You're already logged in.")
			return
		end
		
		if Auth.Login(sender, GetSenderUserHostString(), params) == true then
			SendMessage(sender, string.format([[You are now logged in with the following flags: %s]], Auth.GetFlags(sender)))
		else
			SendMessage(sender, "Authentication failed.")
		end
	end,
	
	HandleLogoutCommand = function(sender, channel, params)
		if not Auth.IsAuthed(sender, GetSenderUserHostString()) then
			SendMessage(channel, "You're not logged in!")
			return
		end
		
		Auth.Logout(sender)
		SendMessage(channel, "You have been logged out.")
	end,
}

Commands.RegisterCommand("help", Commands.HandleHelpCommand, [[Lists all of the available commands available, and provides lists help text for other commands]])
Commands.RegisterCommand("login", Commands.HandleLoginCommand, [[Logs in a registered user so that they can use privileged commands]])
Commands.RegisterCommand("logout", Commands.HandleLogoutCommand, [[Logs you out]])

Commands.RegisterCommand("register", function(sender, channel, params)
	if channel:sub(1,1) == "#" then
		SendMessage(channel, "You can't register in a channel.")
		return
	end
	
	if params:len() < 4 then
		SendMessage(sender, "Your password is too short.")
		return
	end
	
	Auth.AddUser(sender, params, "")
	SendMessage(sender, "Thank you for registering! You can now use the login command to authenticate yourself. Please remember your password.")
end, [[Registers your nickname with this bot]])

Commands.RegisterCommand("addflags", function(sender, channel, params)
	local space = params:find(" ")
	local to = ""
	if space ~= nil then
		to = params:sub(1, space-1)
	end
	
	local flags = params:sub(space+1)
	if to == nil or flags == "" or flags == nil then
		SendMessage(channel, "Incorrect format. Format is: addflags USER FLAGS")
		return
	end
	
	if not Auth.IsRegistered(to) then
		SendMessage(channel, "That user isn't registered.")
		return
	end
	
	Auth.AddFlag(to, flags)
	SendMessage(channel, string.format([[Added flags to %s successfully.]], to))
end, [[Adds flags to a registered username.]], "z")

Commands.RegisterCommand("quit", function(sender, channel, params)
	if params ~= nil and params ~= "" then
		Quit(params)
	else
		Quit("Bye!")
	end
end, [[Instructs this bot to quit immediately]], "z")

Commands.RegisterCommand("version", function(sender, channel, params)
	SendMessage(channel, "I am running " .. GetVersionInfo())
end, [[Outputs version information for the bot.]])