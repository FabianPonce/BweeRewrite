function OnMessageReceived (sender, channel, message)
 	print( "I received a message from " .. sender .. "!")
	SendMessage("#bwee", sender .. " said: " .. message);
end

RegisterMessageHandler(OnMessageReceived)

function OnConnected ()
	print "I just connected to a server!"
	Join("#bwee")
end
RegisterConnectedHandler(OnConnected)