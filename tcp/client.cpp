#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "client.hpp"

TCPConnection *TCPClient::connect(const std::string &host, int port) throw(TCPException)
{
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	portno = port;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		throw TCPException("Error opening socket");
	}
	
	server = gethostbyname(host.data());
	if(server == NULL)
	{
		throw TCPException("Error, no such host");
	}
	
	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	
	if(::connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		throw TCPException("Error connecting");
	}
	
	return new TCPConnection(sockfd);
}

void TCPClient::disconnect(TCPConnection *conn) throw(TCPException)
{
	if(conn == nullptr)
	{
		throw TCPException("TCPConnection is NULL");
	}
	if(close(conn->get_fd()) < 0)
	{
		throw TCPException("Error closing socket");
	}
}
