#ifndef Bwee_IRCSession_h
#define Bwee_IRCSession_h

#include "Common.h"

#define SESSION_UPDATE_RESOLUTION 100

#define ADD_MESSAGEHANDLER(code,method) \
	m_messageMap.insert( make_pair(code, method) );

class IRCSession;

struct IRCMessagePrefix
{
	/*
	 * This structure represents the optional prefix at the head of a message:
	 *	ie: nick!user@c-1-2-3-4.hsd1.nj.comcast.net
	 */
	std::string nickOrServerName;
	std::string userName;
	std::string hostName;

	IRCMessagePrefix(std::string rawPrefix)
	{
		size_t exclPos = rawPrefix.find('!');
		if( exclPos != string::npos )
		{
			// This is a message from another client, relayed via the server.
			nickOrServerName = rawPrefix.substr(0, exclPos);
			size_t atPos = rawPrefix.find('@');
			userName = rawPrefix.substr(exclPos+1,atPos-exclPos-1);
			hostName = rawPrefix.substr(atPos+1);
		} else {
			// This is a server-based message.
			nickOrServerName = rawPrefix;
			userName = "";
			hostName = rawPrefix;
		}
	}

	IRCMessagePrefix(const char* pNickOrServer, const char* pUserName, const char* pHostName)
	{
		nickOrServerName = pNickOrServer;
		userName = pUserName;
		hostName = pHostName;
	}

	std::string toString()
	{
		std::stringstream ss;
		ss << nickOrServerName << "!" << userName << "@" << hostName;
		return ss.str();
	}
};

#define MAX_PARAMETERS 15

struct IRCMessage
{
	IRCMessagePrefix* prefix;
	std::string rawPrefix; // READ-ONLY

	std::string command;
	std::list<std::string> params;
	std::string rawParams; // READ-ONLY
	std::string trailing;

	IRCMessage(std::string pCommand, std::string pParam)
	{
		command = pCommand;
		params.push_back(pParam);
		prefix = NULL;
	}

	IRCMessage()
	{
		prefix = NULL;
	}

	~IRCMessage()
	{
		if(prefix)
			delete prefix;
	}

	bool hasPrefix()
	{
		return rawPrefix.size() != 0;
	}

	bool hasTrailing()
	{
		return trailing.size() != 0;
	}

	bool hasParams()
	{
		return params.size() != 0;
	}
	
	std::string toString()
	{
		std::stringstream msg;
		if( hasPrefix() )
			msg << ":" << rawPrefix << " ";

		msg << command;

		if( params.size() ) 
		{
			std::list<std::string>::iterator itr = params.begin();
			for(; itr != params.end(); ++itr)
			{
				msg << " " << (*itr);
			}
		}

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

	void SendMessage(IRCMessage* pMessage);
	void SendMessage(IRCMessage& pMessage);

	// Message Handlers
	void HandlePing(IRCMessage& recvData);
	void Handle001(IRCMessage& recvData); // 001: Successful Registration (http://www.mirc.net/raws/?view=001)
	void HandlePrivmsg(IRCMessage& recvData);

	SimpleSocket* m_socket;

	MessageHandlerMap m_messageMap;
};

#endif
