require "commands"
require "customcommands"
require "njnew"

function OnMessageReceived (sender, channel, message)
	-- If 'channel' is our nickname, then this is a private message, and we should respond to the sender, not ourselves.
	if channel == GetNick() then
		channel = sender
	end
	
	Notes.ProcessNotes(channel, sender)
	
	-- Respond to both commands issued with !command or .command syntax
	if (message:sub(1, 1) == "!" or message:sub(1,1) == ".") and (message:sub(1,1) ~= message:sub(2,2)) then
		-- This is a command!
		local nextSpace = message:find(" ")
		if nextSpace == nil then
			nextSpace = message:len()
		else
			nextSpace = nextSpace - 1
		end
	
		local command = message:sub(2, nextSpace)
		local parameters = ""
		if nextSpace ~= message:len() then
			parameters = message:sub(nextSpace+2)
		end
	
		Commands.TriggerCommand(command, sender, channel, parameters)
		return
	end
	
	-- If we've reached here, it's not a command or anything the command processor wants to handle.
	
	-- Insert stupid fun shit here!
	local responses = {
		"Did someone say my name?",
		"Please. I know I have a vagina, but I don't have to be ALL you talk about.",
		"I'm napping, go away!",
		"So this one time, I beat up kensia with a tire iron.",
		"Yes, I do find you attractive!",
		string.char(1) .. "ACTION flips a table. (╯°□°)╯︵ ┻━┻" .. string.char(1),
		"Nuclear devices activated, and the machine keeps pushing time through the cogs, like paste into strings into paste again, and only the machine keeps using time to make time to make time.",
		"Are you my mummy?",
	}
	if (message:lower():find("hi ") ~= nil or message:lower():find("hey ") ~= nil or message:lower():find("hello ") ~= nil) and message:lower():find(GetNick():lower()) ~= nil then
		SendMessage(channel, responses[math.random(#responses)])
		return
	end
	
	if message:find(string.char(1) .. "ACTION flips a table") ~= nil then
		SendMessage(channel, "(╯°□°)╯︵ ┻━┻")
		return
	end

	if channel == "#newjersey" and NJNew.CanPullXML() then
		local posts = NJNew.GetNewPosts()
		for i,post in pairs(posts) do
			SendMessage(channel, string.format([[::njnew:: %s @ %s]], post.title, post.link))
		end
	end
end

RegisterMessageHandler(OnMessageReceived)

function OnConnected ()
	print "I just connected to a server!"
	Join("#njtest")
end
RegisterConnectedHandler(OnConnected)

function TopicChanged (channel, topic, changedby)
	print("The topic of " .. channel .. " is: " .. topic)
	-- SendMessage(channel, "I like this new topic.")
end
RegisterTopicChangedHandler(TopicChanged)

function ReceivedMotd (newMotd)
	print ("I have received the MOTD!")
	print (newMotd)
end
RegisterMotdHandler(ReceivedMotd)

function ChannelJoined(nick, channel)
	if nick == GetNick() then
		SendMessage(channel, "I'm back, guys!")
		return
	end
	
	-- SendMessage(channel, string.format(
	-- 	"Welcome to %s, %s! =]", channel, nick
	-- ))
	
	Notes.ProcessNotes(channel, nick)
end
RegisterChannelJoinedHandler(ChannelJoined)

function LUAReloaded ()
	print "LUA has been reloaded successfully."
end
RegisterLUAReloadedHandler(LUAReloaded)