#include "Common.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << Util::getVersionString() << std::endl;
    
    
    IRCSession session("irc.freenode.net", 6667);

#ifdef WIN32
	WSACleanup();
#endif
    
    return 0;
} 