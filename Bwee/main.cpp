#include "Common.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << Util::getVersionString() << " | " << PLATFORM_TEXT << std::endl;
    
    
    SimpleSocket sock("localhost", 123);
    if(!sock.isConnected())
    {
        std::cout << "Could not connect!" << std::endl;
        return 0;
    }
    
    if(sock.sendLine("this is a test!"))
    {
        std::cout << "Message was sent!";
    } else {
        std::cout << "Message was not sent! :(";
    }
    
    while(true)
    {
        if( sock.hasLine() ) {
            std::string line = sock.readLine();
            sock.sendLine(line); // send it back to verify
            break;
        }
    }

#ifdef WIN32
	WSACleanup();
#endif
    
    return 0;
} 