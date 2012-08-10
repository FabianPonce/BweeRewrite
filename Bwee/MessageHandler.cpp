#include "Common.h"

void IRCSession::HandlePing(IRCMessage& recvData)
{
	// Command: PING
	// Parameters: <server1> [<server2>]
	SendMessage(MessageFactory::Pong(recvData.rawParams.c_str()));
}

void IRCSession::Handle001(IRCMessage& recvData)
{
	// We receive this message once registration was successful, NICK/USER are authenticated.
	// Join a default channel for now?

	SendMessage(MessageFactory::Join("#bwee"));
}

void IRCSession::HandlePrivmsg(IRCMessage& recvData)
{
	
}