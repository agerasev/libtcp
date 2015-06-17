#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <list>

#include <unistd.h>
#include <netinet/in.h>

#include "connection.hpp"
#include "exception.hpp"

class TCPServer
{
private:
	struct TCPThread
	{
		std::thread *thread = nullptr;
		int sockfd;
		bool active = false;
	};
	
	int sockfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	
	std::thread *listener;
	std::list<TCPThread> pool;
	std::mutex mutex;
	
	std::function<void(TCPConnection *)> handler;
	std::function<void(const std::string &)> error_handler;
	bool active = false;
	
public:
	TCPServer(
	    int port, 
	    std::function<void(TCPConnection *)> _handler,
	    std::function<void(const std::string &)> _error_handler
	) throw(TCPException);
	
	~TCPServer() throw(TCPException);
};
