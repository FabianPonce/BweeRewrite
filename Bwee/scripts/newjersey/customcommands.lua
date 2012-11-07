require "commands"
require "notes"
require "quotes"
require "njnew"

Commands.RegisterCommand("reloadlua", function(sender, channel, params)
	SendMessage(channel, "One moment while I re-learn everything I once knew.")
	Storage.Cleanup()
	ReloadLUA()
end, [[Reloads the LUA scripting engine.]], "z")

Commands.RegisterCommand("getquote", function(sender, channel, params)
	local requestedQuote = tonumber(params)
	if requestedQuote ~= nil then
		local id, sender, message = Quotes.GetQuoteByID(requestedQuote)
		if id ~= nil then
			SendMessage(channel, string.format("[#%u (%s)] %s", id, sender, message))
		else
			SendMessage(channel, string.format("No quote found with ID: #%u.", requestedQuote))
		end
	else
		local id, sender, message = Quotes.GetAnyQuote()
		if id ~= nil then
			SendMessage(channel, string.format("[#%u (%s)] %s", id, sender, message))
		else
			SendMessage(channel, "No quotes are in my database yet!")
		end
	end
end, [[Gets a random quote, or if specified, a quote by it's ID]])

Commands.RegisterCommand("addquote", function(sender, channel, params)
	local quote = params
	if quote:len() < 3 then
		SendMessage(channel, string.format([[Sorry, %s, that quote is too short.]], sender))
	else
		local id = Quotes.AddQuote(sender, quote)
		SendMessage(channel, string.format("Okay, added quote #%u.", id))
	end
end, [[Adds a quote to the database.]])

Commands.RegisterCommand("note", function(sender, channel, params)
	local space = params:find(" ")
	local to = ""
	if space ~= nil then
		to = params:sub(1, space-1)
	end
	
	if to == "" or params:sub(space+1) == nil then
		SendMessage(channel, "Insufficient parameters.")
	else
		Notes.AddNote(to, sender, params:sub(space+1))
		SendMessage(channel, string.format("I'll get that to %s right away, %s.", to, sender))
	end
end, [[Sends a message to a user the next time this bot sees them.]])

Commands.RegisterCommand("join", function(sender, channel, params)
	if params:sub(1,1) ~= "#" then
		SendMessage(channel, "That's not a channel, silly.")
	else
		SendMessage(channel, string.format("Sure, I'll head on over to %s now.", params))
		Join(params)
	end
end, [[Instructs this bot to join a new channel]], "a")

Commands.RegisterCommand("say", function(sender, channel, params)
	if params:sub(1,1) ~= "#" then
		SendMessage(channel, params)
	else
		local spaceLoc = params:find(" ")
		if spaceLoc == nil then
			SendMessage(channel, "You didn't specify a channel!")
			return
		end
		local target = params:sub(1, spaceLoc-1)
		local message = params:sub(spaceLoc+1)
		
		SendMessage(target, message)
	end
end, [[Makes this bot say something to a specified channel (optional)]], "a")

Commands.RegisterCommand("8ball", function(sender, channel, params)
	local responses = {
		"It is certain", "Without a doubt", "Yes - definitely", "Most likely", "Yes", "Signs point to yes", "Ask again later", "Better not tell you now",
		"Concentrate and ask again", "Don't count on it", "My reply is no", "My sources say no", "Outlook not so good", "Very doubtful"
	}
	SendMessage(channel, responses[math.random(#responses)])
end, [[Ask the bot a question with this command]])

Commands.RegisterCommand("1337", function(sender, channel, params)
	if params == "" then
		SendMessage(channel, "Come on, give me something to work with at least.")
		return
	end
	
	local replacements = {
		["a"] = "4",
		["s"] = "$",
		["t"] = "7",
		["l"] = "1",
		["z"] = "2",
		["s"] = "5",
		["e"] = "3",
		["i"] = "1",
		["o"] = "0",
	}
	
	params = string.gsub(params, "%a",
		function(str)
			if replacements[str] then
				return replacements[str]
			elseif replacements[str:lower()] then
				return replacements[str:lower()]
			end
		end
	)
	
	SendMessage(channel, params)
end, [[Converts your speech to 1337speak]])

Commands.RegisterCommand("roulette", function(sender, channel, params)
	if Roulette == nil then
		Roulette = { }
		Roulette.cur = 1
		Roulette.trigger = math.random(1,6)
	end
	
	if Roulette.cur == Roulette.trigger then
		SendMessage(channel, string.format([[(%s) Bang! You're dead.]], sender))
		SendMessage(channel, string.char(1) .. "ACTION reloads." .. string.char(1))
		Roulette.cur = 1
		Roulette.trigger = math.random(1,6)
	else
		Roulette.cur = Roulette.cur + 1
		if Roulette.cur == 6 then
			SendMessage(channel, string.format([[Click. (%u rounds remain)]], 6 - Roulette.cur + 1))
			SendMessage(channel, string.char(1) .. "ACTION reloads." .. string.char(1))
			Roulette.cur = 1
			Roulette.trigger = math.random(1,6)
		else
			SendMessage(channel, string.format([[Click. (%u rounds remain)]], 6 - Roulette.cur + 1))
		end
	end
end, [[Plays a game of Russian roulette]])

Commands.RegisterCommand("njnew", function(sender, channel, params)
	local new = NJNew.GetNewPosts()
	if new == nil or #new == 0 then
		SendMessage(channel, "No new r/newjersey posts.")
		return
	end
	
	for i,post in pairs(new) do
		SendMessage(channel, string.format([[::njnew:: %s @ %s]], post.title, post.link))
	end
end, [[Performs a manual NJNew update]], "a")
