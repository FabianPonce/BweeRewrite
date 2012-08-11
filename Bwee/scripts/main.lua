-- ----------------------------------------------------------------------------------------------
-- This is a valid LUA 5.1 script.
-- ----------------------------------------------------------------------------------------------
-- API Documentation for Bwee
-- ----------------------------------------------------------------------------------------------
--
-- Event Registration:
-- 
-- 	RegisterMessageHandler(<function a>)
--		Designates a function that is called when the bot receives a PRIVMSG.
--		
--		<function a> parameters: sender, channel, message
--
--	RegisterConnectedHandler(<function a>)
--		Designates a function that is called when the bot connects successfully to an IRCd.
--
--	RegisterTopicChangedHandler(<function a>)
--		Designates a function that is called whenever the bot receives information about a topic for a channel.
--		This includes newly joined channels, and topic changes.
--		
--		<function a> parameters: channel, topic, changedby
--
--	
-- Accessors:
--
--	GetNick()
--		Returns the current nickname of the bot.
--
--	GetVersionInfo()
--		Returns a string representing the current version of Bwee and platform the bot is running on
--
--
-- Mutators:
--
--	SendMessage(target, message)
--		Sends an ordinary PRIVMSG to the target, with message.
--
--	Quit([message])
--		Terminates the connection to the IRC server and sends a quit message, if specified.
--
--	Join(channel, [key])
--		Joins a channel on the IRC server. If the channel is password-protected, the optional key parameter will be used.
--		
-- ----------------------------------------------------------------------------------------------
-- Example code provided below.
-- ----------------------------------------------------------------------------------------------

function OnMessageReceived (sender, channel, message)
 	if string.find(message, "hello") then
		SendMessage(channel, "Hello, " .. sender .. ", nice to meet you, I'm " .. GetNick())
	elseif string.find(message, "version") then
		SendMessage(channel, "I am running " .. GetVersionInfo())
	elseif string.find(message, "quit") then
		Quit("Bye!")
	end
end

RegisterMessageHandler(OnMessageReceived)

function OnConnected ()
	print "I just connected to a server!"
	Join("#bwee")
end
RegisterConnectedHandler(OnConnected)

function TopicChanged (channel, topic, changedby)
	print("The topic of " .. channel .. " is: " .. topic)
	SendMessage(channel, "I like this new topic.")
end
RegisterTopicChangedHandler(TopicChanged)