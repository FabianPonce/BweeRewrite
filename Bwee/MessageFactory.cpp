#include "Common.h"

IRCMessage* MessageFactory::Pong(const char* serverList)
{
	return new IRCMessage("PONG", serverList);
}

IRCMessage* MessageFactory::NickName(const char* pNickName)
{
	return new IRCMessage("NICK", pNickName);
}

IRCMessage* MessageFactory::User(const char* pUserName, const char* pHostName, const char* pServerName, const char* pRealName)
{
	IRCMessage* msg = new IRCMessage;
	msg->command = "USER";
	msg->trailing = pRealName;
	msg->params.push_back(pUserName);
	msg->params.push_back(pHostName);
	msg->params.push_back(pServerName);

	return msg;
}

IRCMessage* MessageFactory::Join(const char* channel, const char* key /* = "" */)
{
	IRCMessage* msg = new IRCMessage;
	msg->command = "JOIN";
	msg->params.push_back(channel);
	if( strlen(key) > 0 )
	{
		msg->params.push_back(key);
	}

	return msg;
}

IRCMessage* MessageFactory::Quit(const char* pReason)
{
	IRCMessage* msg = new IRCMessage;
	msg->command = "QUIT";
	msg->trailing = pReason;
	return msg;
}

IRCMessage* MessageFactory::PrivMsg(const char* pTarget, const char* pMessage)
{
	IRCMessage* msg = new IRCMessage;
	msg->command = "PRIVMSG";
	msg->params.push_back(pTarget);
	msg->trailing = pMessage;
	return msg;
}