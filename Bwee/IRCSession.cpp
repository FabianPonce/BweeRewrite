#include "IRCSession.h"

IRCSession::IRCSession(std::string pServer, uint32 pPort)
{
	// Setup the message mappings
	ADD_MESSAGEHANDLER("PING", &IRCSession::HandlePing);
	ADD_MESSAGEHANDLER("001", &IRCSession::Handle001);
	ADD_MESSAGEHANDLER("PRIVMSG", &IRCSession::HandlePrivmsg);

	m_socket = new SimpleSocket(pServer, pPort);

	Update();
}

IRCSession::~IRCSession()
{
	SendMessage(MessageFactory::Quit(Util::getVersionString().c_str()));

	delete m_socket;
	m_socket = NULL;
}

void IRCSession::Parse(std::string pMessage)
{
	std::cout << pMessage << std::endl;

	/*
	 * Any message we receive will be in the following format:
	 *	:<prefix> <command> <params> :<trailing>
	 * >> prefix and trailing parts are optional. If the prefix or trailing part is missing, there will also be missing ':'
	 */
	IRCMessage message;

	// We need to determine if the prefix is present or not.
	size_t prefixOffset = 0;
	if( pMessage[0] == ':' )
	{
		// Read in the prefix
		prefixOffset = pMessage.find(' ');
		message.rawPrefix = pMessage.substr(1, prefixOffset-1);
		message.prefix = new IRCMessagePrefix(message.rawPrefix);

		// increment prefixOffset only if there was a prefix, so that it points to the next word
		++prefixOffset; 
	}

	size_t postCommandSpace = pMessage.find(' ', prefixOffset);
	message.command = pMessage.substr(prefixOffset, postCommandSpace-prefixOffset);
	size_t postPrefixSpace = pMessage.find(' ', postCommandSpace+1);
	message.params = pMessage.substr(postCommandSpace+1, postPrefixSpace-postCommandSpace-1);
	
	size_t trailingMarker = pMessage.find(':', postPrefixSpace);
	if( trailingMarker != string::npos )
		message.trailing = pMessage.substr(trailingMarker+1);

	HandleMessage(&message);
}

void IRCSession::HandleMessage(IRCMessage* pMessage)
{
	MessageHandlerMap::iterator itr = m_messageMap.find(pMessage->command);
	if( itr != m_messageMap.end() )
	{
		IRCSessionMessageHandler cb = itr->second;
		(this->*cb)(*pMessage);
	} else {
		std::cout << "Unhandled IRC command: " << pMessage->command << std::endl;
	}
}

void IRCSession::Update()
{
	/*
	 * If we're at this point we haven't sent anything into the socket
	 * initialize the connection with NICK and USER.
	 */
	SendMessage(MessageFactory::NickName("Bwee"));
	SendMessage(MessageFactory::User("Bwee", "localhost", "localhost", "Bwee"));

	while(m_socket->isConnected() || m_socket->hasLine())
	{
		if( m_socket->hasLine() )
		{
			std::string msg = m_socket->readLine();
			Parse(msg);
		}
		Sleep(SESSION_UPDATE_RESOLUTION);
	}
}

void IRCSession::SendMessage(IRCMessage* pMessage)
{
	m_socket->sendLine(pMessage->toString());
	delete pMessage;
}

void IRCSession::SendMessage(IRCMessage& pMessage)
{
	m_socket->sendLine(pMessage.toString());
}