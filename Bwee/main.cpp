#include "Common.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << Util::getVersionString() << std::endl;
    
    
    IRCSession session("irc.freenode.net", 6667);

#ifdef WIN32
	WSACleanup();
#endif
    
	std::cout << "All IRC connections have been terminated." << std::endl;
	std::cout << "Terminating in 5 seconds..." << std::endl;

	Sleep(5000);
    return 0;
} 