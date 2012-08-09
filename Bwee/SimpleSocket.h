#ifndef Bwee_Socket_h
#define Bwee_Socket_h

#include "Common.h"

class SimpleSocket
{
public:
    /*
     * Constructions a new instance of SimpleSocket and attempts to connect to the 
     * host and port passed. 
     *
     * Call SimpleSocket::isConnected() afterwards to determine if this was successful.
     */
    SimpleSocket(std::string pHost, uint32 pPort);
    ~SimpleSocket();
    
    /*
     * Returns whether or not this socket is active.
     */
    bool isConnected();
    
    /* 
     * Writes a string to the remote host. A newline will be appended to the pLine parameter.
     */
    bool sendLine(std::string pLine);
    
    /*
     * Returns true if there is data to be fetched from the remote host, via the SimpleSocket::readLine() call.
     * This method can block for an extensive period of time.
     */
    bool hasLine();
    
    /*
     * Returns a single string from the input buffer, delimited by a newline. 
     * This method should not be called unless SimpleSocket::hasLine() has already been called to avoid undefined behavior.
     */
    std::string readLine();
    
    
protected:
#ifndef WIN32
    int m_fd;
#else
	SOCKET m_fd;
#endif
    std::string m_buffer;
};

#endif
