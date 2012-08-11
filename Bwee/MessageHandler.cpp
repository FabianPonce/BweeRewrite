#include "Common.h"

void IRCSession::HandlePing(IRCMessage& recvData)
{
	// Command: PING
	// Parameters: <server1> [<server2>]
	SendMessage(MessageFactory::Pong(recvData.rawParams.c_str()));
}

void IRCSession::Handle001(IRCMessage& recvData)
{
	m_nickName = recvData.rawParams;

	// We receive this message once registration was successful, NICK/USER are authenticated.
	m_scriptInterface->OnConnected();
}

void IRCSession::HandlePrivmsg(IRCMessage& recvData)
{
	m_scriptInterface->OnReceivedMessage(recvData.prefix->nickOrServerName.c_str(), recvData.rawParams.c_str(), recvData.trailing.c_str());
}

void IRCSession::HandleReplyTopic(IRCMessage& recvData)
{
	if( recvData.command == MESSAGE_RPL_NOTOPIC )
		m_scriptInterface->OnChangedTopic(recvData.rawParams.c_str(), "", recvData.prefix->nickOrServerName.c_str());
	else
		m_scriptInterface->OnChangedTopic(recvData.rawParams.c_str(), recvData.trailing.c_str(), recvData.prefix->nickOrServerName.c_str());
}

void IRCSession::HandleMotdMessages(IRCMessage& recvData)
{
	if( recvData.command == MESSAGE_RPL_MOTDSTART ) {
		m_motd.clear();
		m_motdIsDone = false;
	}
	else if( recvData.command == MESSAGE_RPL_MOTD ) { // RPL_MOTD is in format ":- <text>"
		m_motd.append(recvData.trailing.substr(2));
		m_motd.append("\n");
	}
	else if( recvData.command == MESSAGE_RPL_ENDOFMOTD)
	{
		m_motdIsDone = true;
		m_scriptInterface->OnReceivedMotd(getMotd());
	}
}