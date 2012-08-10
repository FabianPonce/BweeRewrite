#include "Common.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << Util::getVersionString() << " | " << PLATFORM_TEXT << std::endl;
    
    
    IRCSession session("irc.freenode.net", 6667);

#ifdef WIN32
	WSACleanup();
#endif
    
    return 0;
} 