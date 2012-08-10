#ifndef Bwee_IRCSession_h
#define Bwee_IRCSession_h

#include "Common.h"

#define SESSION_UPDATE_RESOLUTION 100

#define ADD_MESSAGEHANDLER(code,method) \
	m_messageMap.insert( make_pair(code, method) );

class IRCSession;

struct IRCMessage
{
	std::string prefix;

	std::string command;
	std::string params;
	std::string trailing;

	IRCMessage(std::string pCommand, std::string pParams)
	{
		command = pCommand;
		params = pParams;
	}

	IRCMessage()
	{

	}

	bool hasPrefix()
	{
		return prefix.size() != 0;
	}

	bool hasTrailing()
	{
		return trailing.size() != 0;
	}
	
	std::string toString()
	{
		std::stringstream msg;
		if( hasPrefix() )
			msg << ":" << prefix << " ";

		msg << command << " " << params;

		if( hasTrailing() )
			msg << " :" << trailing;

		return msg.str();
	}
};

typedef void(IRCSession::*IRCSessionMessageHandler)(IRCMessage& recvData);
typedef std::map<std::string, IRCSessionMessageHandler> MessageHandlerMap;

/*
 * This class implements the main functionality of connecting and managing a connection to an
 * IRC server. 
 *
 * This should implement RFC 1459 wherever possible, though given the small nature of
 * this project, some deviation is expected.
 * <http://www.irchelp.org/irchelp/rfc/rfc.html>
 */
class IRCSession
{
public:

	IRCSession(std::string pServer, uint32 pPort);
	~IRCSession();

protected:
	void Update();
	void Parse(std::string pMessage);
	void HandleMessage(IRCMessage* pMessage);

	void SendIRCMessage(IRCMessage* pMessage);
	void SendIRCMessage(IRCMessage& pMessage);

	// Message Handlers
	void HandlePing(IRCMessage& recvData);
	void Handle001(IRCMessage& recvData); // 001: Successful Registration (http://www.mirc.net/raws/?view=001)

	SimpleSocket* m_socket;

	MessageHandlerMap m_messageMap;
};

#endif
