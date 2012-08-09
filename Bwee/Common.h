#ifndef Bwee_Common_h
#define Bwee_Common_h

#define BWEE_MAJOR_VERSION 0
#define BWEE_MINOR_VERSION 1
#define BWEE_BUGFIX_VERSION 0

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#else
#include <WinSock2.h>
#endif

// current platform and compiler
#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2

// platform detection
#define UNIX_FLAVOUR_LINUX 1
#define UNIX_FLAVOUR_BSD 2
#define UNIX_FLAVOUR_OTHER 3
#define UNIX_FLAVOUR_OSX 4

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define PLATFORM PLATFORM_WIN32
#elif defined( __APPLE__ )
#  define PLATFORM PLATFORM_APPLE
#  include <TargetConditionals.h>
#else
#  define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_APPLE
#ifdef TARGET_OS_MAC
#define PLATFORM_TEXT "MacOSX"
#define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#else
#ifdef USE_KQUEUE
#define PLATFORM_TEXT "FreeBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#define PLATFORM_TEXT "Linux"
#define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#endif
#endif
#else
#if PLATFORM == PLATFORM_WIN32
#define PLATFORM_TEXT "Win32"
#endif
#endif


#ifdef WIN32
typedef signed __int64 int64;
typedef signed __int32 int32;
typedef signed __int16 int16;
typedef signed __int8 int8;

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;

typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;
#else
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef uint32_t DWORD;
#endif



#include "SimpleSocket.h"
#include "Util.h"


#endif
