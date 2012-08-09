#include "Common.h"

SimpleSocket::SimpleSocket(std::string pHost, uint32 pPort)
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_fd < 0) {
        m_fd = 0; // out of sockets on the host?
        return;
    }
    
    struct sockaddr_in remote_addr;
    struct hostent *server;
    
    server = gethostbyname(pHost.c_str());
    if(!server)
    {
        m_fd = 0; // can't connect to a host we can't look up.
        return;
    }
    
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    memcpy((void*)server->h_addr, &remote_addr.sin_addr.s_addr, server->h_length);
    remote_addr.sin_port = htons(pPort);
    
    if (connect(m_fd,(struct sockaddr *) &remote_addr,sizeof(remote_addr)) < 0) {
        m_fd = 0; 
        // connection failed!
    }
}

SimpleSocket::~SimpleSocket()
{
    if(m_fd)
        close(m_fd);
    
    m_fd = 0;
}

bool SimpleSocket::isConnected()
{
    return m_fd != 0;
}

bool SimpleSocket::sendLine(std::string pLine)
{
    if( !isConnected() )
        return false;
    
    pLine.append("\n");
    
    int n = write(m_fd, pLine.c_str(), (int)pLine.size());
    if(n < 0)
        return false;
    
    return true;
}

bool SimpleSocket::hasLine()
{
    int res;
    char buf[4096];
    /*
     * This recv call will block for a while if there's no data immediately available.
     * Consider migrating to non-blocking IO?
     */
    res = recv(m_fd, buf, sizeof(buf), 0);
    if(res < 0)
    {
        m_fd = 0;
        return false;
    } else if(res > 0) {
        std::cout << "Got input";
        m_buffer.append(buf, res);
    }
    
    if( m_buffer.find("\n") != string::npos )
        return true;
    
    return false;
}

std::string SimpleSocket::readLine()
{
    size_t loc = m_buffer.find("\n");
    std::string line = m_buffer.substr(0, loc);
    m_buffer.erase(0, loc+1); // +1 to remove the \n as well!
    return line;
}