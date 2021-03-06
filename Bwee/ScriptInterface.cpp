#include "Common.h"

static std::map<lua_State*, IRCSession*> m_sessionMap;
static std::map<lua_State*, ScriptInterface*> m_interfaceMap;

#define SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(eventName, scriptEnum, ...) \
	void ScriptInterface::eventName(__VA_ARGS__) \
{ \
	for(std::set<uint16>::iterator itr = m_eventHandlers[scriptEnum].begin(); itr != m_eventHandlers[scriptEnum].end(); ++itr) \
{ \
	uint16 function = (*itr); \
	prepareFunction(function);


#define SCRIPTINTERFACE_END_EVENT_HANDLER \
	executeFunction(); \
} \
}

ScriptInterface::ScriptInterface(IRCSession* pSession)
{
	m_session = pSession;
	m_luaState = lua_open();
	if(!m_luaState)
	{
		std::cerr << "Unable to initialize LUA engine!" << std::endl;
		return;
	}

	m_sessionMap.insert( make_pair(m_luaState, m_session) );
	m_interfaceMap.insert( make_pair(m_luaState, this) );
	
	luaL_openlibs(m_luaState);
	
	setPath("scripts/?.lua");
	setPath("scripts/deps/?.lua");

	registerFunctions();

	int e = luaL_loadfile(m_luaState, "scripts/main.lua");

	if( e == 0 ) // Execute zeh program!
		e = lua_pcall(m_luaState, 0, LUA_MULTRET, 0);

	reportErrors(e);
}

ScriptInterface::~ScriptInterface()
{
	{
		std::map<lua_State*, ScriptInterface*>::iterator itr = m_interfaceMap.find(m_luaState);
		if( itr != m_interfaceMap.end() )
			m_interfaceMap.erase(itr);
	}
	
	{
		std::map<lua_State*, IRCSession*>::iterator itr = m_sessionMap.find(m_luaState);
		if( itr != m_sessionMap.end() )
			m_sessionMap.erase(itr);
	}


	lua_close(m_luaState);
	m_luaState = NULL;
}

void ScriptInterface::setPath(const char* path)
{
	lua_getglobal( m_luaState, "package" );
    lua_getfield( m_luaState, -1, "path" ); // get field "path" from table at top of stack (-1)
    std::string cur_path = lua_tostring( m_luaState, -1 ); // grab path string from top of stack
    cur_path.append( ";" ); // do your path magic here
    cur_path.append( path );
    lua_pop( m_luaState, 1 ); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring( m_luaState, cur_path.c_str() ); // push the new one
    lua_setfield( m_luaState, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
    lua_pop( m_luaState, 1 ); // get rid of package table from top of stack
}

inline void ScriptInterface::prepareFunction(uint16 func)
{
	lua_settop(m_luaState, 0); // Empty the stack
	lua_getref(m_luaState, func);
	m_argCount = 0;
}

inline void ScriptInterface::executeFunction()
{
	int e = lua_pcall(m_luaState, m_argCount, 0, 0);
	reportErrors(e);

	m_argCount = 0;
}

inline void ScriptInterface::reportErrors(int pStatus)
{
	if( pStatus != 0 )
	{
		std::cout << "-- " << lua_tostring(m_luaState, -1) << std::endl;
		lua_pop(m_luaState, 1);
	}
}

SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(OnReceivedMessage,SCRIPT_EVENT_PRIVMSG,const char* sender, const char* channel, const char* message)
{
	push(sender);
	push(channel);
	push(message);
}
SCRIPTINTERFACE_END_EVENT_HANDLER

SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(OnChangedTopic,SCRIPT_EVENT_TOPICCHANGED, const char* channel, const char* topic, const char* changedBy)
{
	push(channel);
	push(topic);
	push(changedBy);
}
SCRIPTINTERFACE_END_EVENT_HANDLER

SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(OnConnected,SCRIPT_EVENT_CONNECTED)
{
}
SCRIPTINTERFACE_END_EVENT_HANDLER

SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(OnLUAReloaded,SCRIPT_EVENT_LUARELOADED)
{
}
SCRIPTINTERFACE_END_EVENT_HANDLER

SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(OnReceivedMotd, SCRIPT_EVENT_MOTD, const char* motd)
{
	push(m_session->getMotd());
}
SCRIPTINTERFACE_END_EVENT_HANDLER

SCRIPTINTERFACE_BEGIN_EVENT_HANDLER(OnJoinedChannel, SCRIPT_EVENT_CHANNELJOINED, const char* sender, const char* channel)
{
	push(sender);
	push(channel);
}
SCRIPTINTERFACE_END_EVENT_HANDLER

int luaSendMessage(lua_State* pState)
{
	int argc = lua_gettop(pState);
	if(argc != 2)
		return 0;

	IRCSession* sess = m_sessionMap[pState];

	std::string target = lua_tostring(pState, 1);
	std::string message = lua_tostring(pState, 2);

	sess->SendMessage(MessageFactory::PrivMsg(target.c_str(), message.c_str()));

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaJoin(lua_State* pState)
{
	int argc = lua_gettop(pState);
	if(argc == 0 || argc > 2)
		return 0;

	IRCSession* sess = m_sessionMap[pState];

	const char* channel = lua_tostring(pState, 1);
	std::string key = "";
	if( argc == 2 )
		key = lua_tostring(pState, 2);

	sess->SendMessage(MessageFactory::Join(channel, key.c_str()));

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaGetSenderUserHost(lua_State* pState)
{
	int argc = lua_gettop(pState);
	if(argc != 0)
		return 0;

	IRCSession* sess = m_sessionMap[pState];

	lua_pushstring(pState, sess->getCurrentMessage()->prefix->toUserHostString().c_str());
	return 1;
}

int luaQuit(lua_State* pState)
{
	int argc = lua_gettop(pState);
	if(argc > 1)
		return 0;

	IRCSession* sess = m_sessionMap[pState];

	if( argc == 1 )
		sess->Quit(lua_tostring(pState, 1));
	else
		sess->Quit();

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaGetNick(lua_State* pState)
{
	IRCSession* sess = m_sessionMap[pState];
	lua_pushstring(pState, sess->getNickName());

	return 1;
}

int luaReloadLUA(lua_State* pState)
{
	IRCSession* sess = m_sessionMap[pState];
	sess->ReloadLUA();
	return 0;
}


int luaGetVersionInfo(lua_State* pState)
{
	lua_pushstring(pState, Util::getVersionString().c_str());

	return 1;
}

int luaRegisterMessageHandler(lua_State* pState)
{
	lua_settop(pState, 1);

	const char* typeName = luaL_typename(pState, 1);
	if( strcmp(typeName, "function") ) // someones being stupid and passing a string or something
		return 0;

	uint16 function = lua_ref(pState, true);

	m_interfaceMap[pState]->m_eventHandlers[SCRIPT_EVENT_PRIVMSG].insert(function);

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaRegisterTopicChangedHandler(lua_State* pState)
{
	lua_settop(pState, 1);

	const char* typeName = luaL_typename(pState, 1);
	if( strcmp(typeName, "function") ) // someones being stupid and passing a string or something
		return 0;

	uint16 function = lua_ref(pState, true);

	m_interfaceMap[pState]->m_eventHandlers[SCRIPT_EVENT_TOPICCHANGED].insert(function);

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaRegisterConnected(lua_State* pState)
{
	lua_settop(pState, 1);

	const char* typeName = luaL_typename(pState, 1);
	if( strcmp(typeName, "function") ) // someones being stupid and passing a string or something
		return 0;

	uint16 function = lua_ref(pState, true);

	m_interfaceMap[pState]->m_eventHandlers[SCRIPT_EVENT_CONNECTED].insert(function);

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaRegisterMotdHandler(lua_State* pState)
{
	lua_settop(pState, 1);

	const char* typeName = luaL_typename(pState, 1);
	if( strcmp(typeName, "function") ) // someones being stupid and passing a string or something
		return 0;

	uint16 function = lua_ref(pState, true);

	m_interfaceMap[pState]->m_eventHandlers[SCRIPT_EVENT_MOTD].insert(function);

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaRegisterLUAReloadedHandler(lua_State* pState)
{
	lua_settop(pState, 1);

	const char* typeName = luaL_typename(pState, 1);
	if( strcmp(typeName, "function") ) // someones being stupid and passing a string or something
		return 0;

	uint16 function = lua_ref(pState, true);

	m_interfaceMap[pState]->m_eventHandlers[SCRIPT_EVENT_LUARELOADED].insert(function);

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

int luaRegisterChannelJoinedHandler(lua_State* pState)
{
	lua_settop(pState, 1);

	const char* typeName = luaL_typename(pState, 1);
	if( strcmp(typeName, "function") ) // someones being stupid and passing a string or something
		return 0;

	uint16 function = lua_ref(pState, true);

	m_interfaceMap[pState]->m_eventHandlers[SCRIPT_EVENT_CHANNELJOINED].insert(function);

	lua_pop(pState, lua_gettop(pState));
	return 0;
}

void ScriptInterface::registerFunctions()
{
	lua_register(m_luaState, "RegisterMessageHandler", luaRegisterMessageHandler);
	lua_register(m_luaState, "RegisterConnectedHandler", luaRegisterConnected);
	lua_register(m_luaState, "RegisterTopicChangedHandler", luaRegisterTopicChangedHandler);
	lua_register(m_luaState, "RegisterMotdHandler", luaRegisterMotdHandler);
	lua_register(m_luaState, "RegisterLUAReloadedHandler", luaRegisterLUAReloadedHandler);
	lua_register(m_luaState, "RegisterChannelJoinedHandler", luaRegisterChannelJoinedHandler);

	lua_register(m_luaState, "SendMessage", luaSendMessage);
	lua_register(m_luaState, "Join", luaJoin);
	lua_register(m_luaState, "Quit", luaQuit);

	lua_register(m_luaState, "GetNick", luaGetNick);
	lua_register(m_luaState, "GetVersionInfo", luaGetVersionInfo);
	lua_register(m_luaState, "GetSenderUserHostString", luaGetSenderUserHost);

	lua_register(m_luaState, "ReloadLUA", luaReloadLUA);
}