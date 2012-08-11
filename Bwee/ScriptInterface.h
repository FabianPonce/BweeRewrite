#ifndef _Bwee_ScriptInterface_h
#define _Bwee_ScriptInterface_h

#include "Common.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

class IRCSession;

#define SCRIPT_EVENT_PRIVMSG			0
#define SCRIPT_EVENT_CONNECTED			1
#define SCRIPT_EVENT_MAX				2

class ScriptInterface
{
public:
	ScriptInterface(IRCSession* pSession);
	~ScriptInterface();

	void OnReceivedMessage(const char* sender, const char* channel, const char* message);
	void OnConnected();

	std::set<uint16> m_eventHandlers[SCRIPT_EVENT_MAX];
private:
	void registerFunctions();
	void reportErrors(int pStatus);

	IRCSession* m_session;
	lua_State* m_luaState;
};

#endif