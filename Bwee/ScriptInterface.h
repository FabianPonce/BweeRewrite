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
#define SCRIPT_EVENT_TOPICCHANGED		2
#define SCRIPT_EVENT_MOTD				3
#define SCRIPT_EVENT_MAX				4

class ScriptInterface
{
public:
	ScriptInterface(IRCSession* pSession);
	~ScriptInterface();

	void OnReceivedMessage(const char* sender, const char* channel, const char* message);
	void OnChangedTopic(const char* channel, const  char* topic, const char* changedBy);
	void OnConnected();
	void OnReceivedMotd(const char* motd);

	std::set<uint16> m_eventHandlers[SCRIPT_EVENT_MAX];
private:
	lua_State* m_luaState;
	IRCSession* m_session;
	uint32 m_argCount;

	void registerFunctions();

	/*
	 * Reports any errors found during the last invocation of LUA from the C++ engine.
	 */
	inline void reportErrors(int pStatus);

	/*
	 * Instructs the LUA engine to execute the function currently on the stack.
	 * Call after using the prepareFunction() method.
	 */
	inline void executeFunction();

	/*
	 * Prepares a function for execution by placing it at the front of the LUA stack.
	 */
	inline void prepareFunction(uint16 func);

	/*
	 * Push functions to manage the argument counter.
	 */
	void push(const char* pStr) { lua_pushstring(m_luaState, pStr); ++m_argCount; }
	void push(int pInt) { lua_pushinteger(m_luaState, pInt); ++m_argCount; }
};

#endif