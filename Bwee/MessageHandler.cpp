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
	m_scriptInterface->OnConnected();
}

void IRCSession::HandlePrivmsg(IRCMessage& recvData)
{
	m_scriptInterface->OnReceivedMessage(recvData.prefix->nickOrServerName.c_str(), recvData.rawParams.c_str(), recvData.trailing.c_str());
}