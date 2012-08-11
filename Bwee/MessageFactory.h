#ifndef Bwee_MessageFactory_h
#define Bwee_MessageFactory_h

#include "Common.h"

struct IRCMessage;

class MessageFactory
{
public:

	static IRCMessage* NickName(const char* pNickName);

	/* 
	 * Constructs a PONG reply.
	 * The serverList parameter is an optional, space-delimited list of servers to pong.
	 */
	static IRCMessage* Pong(const char* serverList);

	static IRCMessage* User(const char* pUserName, const char* pHostName, const char* pServerName, const char* pRealName);

	static IRCMessage* Join(const char* channel, const char* key = "");

	static IRCMessage* Quit(const char* pReason = "");

	static IRCMessage* PrivMsg(const char* pTarget, const char* pMessage);
};

#endif