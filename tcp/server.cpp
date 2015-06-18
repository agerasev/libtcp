#include "server.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <thread>
#include <functional>

TCPServer::TCPServer(
    int port, 
    std::function<void(TCPConnection *)> _handler, 
    std::function<void(const std::string &)> _error_handler
) throw(TCPException)
{
	int portno;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) 
	{
		throw TCPException("Error opening socket");
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = port;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		throw TCPException("Error on binding");
	}
	
	handler = _handler;
	error_handler = _error_handler;
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	active = true;
	
	listener = new std::thread([this]()
	{
		while(active)
		{
			struct timeval tv;
			fd_set set;
			FD_ZERO(&set);
			FD_SET(sockfd, &set);
			
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			
			int newsockfd = 0;
			
			if(select(sockfd + 1, &set, NULL, NULL, &tv) > 0)
			{
				newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
			}
			else
			{
				continue;
			}
			
			if(newsockfd < 0)
			{
				error_handler("Error on accept");
				continue;
			}
			
			fprintf(stdout,"Connection accepted\n");
			mutex.lock();
			{
				// !! Threads are not removed from list !!
				TCPThread thread;
				thread.sockfd = newsockfd;
				thread.active = true;
				thread.thread = new std::thread([this,&thread]()
				{
					TCPConnection conn = TCPConnection(thread.sockfd);
					handler(&conn);
					close(thread.sockfd);
				});
				pool.push_back(thread);
			}
			mutex.unlock();
		}
		
		fprintf(stdout,"Listener terminated\n");
	});
}

TCPServer::~TCPServer()
{
	active = false;
	listener->join();
	close(sockfd);
	delete listener;
	
	for(TCPThread &thread : pool)
	{
		thread.active = false;
		thread.thread->join();
		delete thread.thread;
	}
}
