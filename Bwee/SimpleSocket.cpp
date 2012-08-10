#include "Common.h"

#ifdef WIN32
bool SIMPLESOCKET_WINSOCK_INIT = false;
#endif

SimpleSocket::SimpleSocket(std::string pHost, uint32 pPort)
{
#ifndef WIN32
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
#else
	if( !SIMPLESOCKET_WINSOCK_INIT )
	{
		SIMPLESOCKET_WINSOCK_INIT = true;
		WSADATA info;
		int e = WSAStartup(MAKEWORD(2,2), &info);
		if(e != 0)
		{
			m_fd = 0;
			return; // unable to initialize WinSock2 library
		}
	}
	m_fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0,0);
#endif

    if(m_fd < 0 || m_fd == ~0) { // windows used a uint, need to check both < 0, and INVALID_SOCKET for cross-platform compat.
        m_fd = 0; // out of sockets on the host?
        return;
    }
    
    struct sockaddr_in remote_addr;
    struct hostent *server = gethostbyname(pHost.c_str());
    if(!server)
    {
        m_fd = 0; // can't connect to a host we can't look up.
        return;
    }
    
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = server->h_addrtype;
	memcpy(&remote_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    remote_addr.sin_port = htons(pPort);
    
    if (connect(m_fd,(struct sockaddr *) &remote_addr,sizeof(remote_addr)) < 0) {
        m_fd = 0; 
        // connection failed!
    }

	/*struct timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	if( setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) == -1 )
		m_fd = 0;*/
}

SimpleSocket::~SimpleSocket()
{
    if(m_fd)
#ifndef WIN32
        close(m_fd);
#else
		closesocket(m_fd);
#endif

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
    
#ifndef WIN32
    int n = write(m_fd, pLine.c_str(), (int)pLine.size());
#else
	int n = send(m_fd, pLine.c_str(), (int)pLine.size(), 0);
#endif
    if(n < 0)
        return false;
    
    return true;
}

bool SimpleSocket::hasLine()
{
	// HACK: This is duplicate code! This is here so that the recv() call will not block indefinitely
	// when there is still data to be read! FIXME
	if( m_buffer.find("\n") != string::npos )
		return true;

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

	if(line[line.size()-1] == '\r')
		line = line.substr(0, line.size()-1);

    return line;
}