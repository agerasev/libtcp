#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "connection.hpp"

static const int READ_LEN  = 0x100;
static const int WRITE_LEN = 0x100;

TCPConnection::TCPConnection(int fd)
  : sockfd(fd)
{
	
}

TCPConnection::~TCPConnection()
{
	
}

int TCPConnection::get_fd() const
{
	return sockfd;
}

void TCPConnection::setTimeout(long to)
{
	timeout = to;
}

long TCPConnection::getTimeout() const
{
	return timeout;
}

void TCPConnection::queue_read_data(char *data, int size)
{
	read_queue.push(std::pair<char *, int>(data,size));
}

void TCPConnection::queue_write_data(const char *data, int size)
{
	write_queue.push(std::pair<const char *, int>(data,size));
}

int TCPConnection::read_size() const
{
	return read_queue.size();
}

int TCPConnection::write_size() const
{
	return write_queue.size();
}

void TCPConnection::perform_read() throw(TCPException,TCPAbortException)
{
	if(sockfd < 0)
	{
		throw TCPException("Invalid connection");
	}
	
	while(read_queue.size())
	{
		struct timeval tv;
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sockfd, &set);
		
		tv.tv_sec = timeout/1000;
		tv.tv_usec = (timeout%1000)*1000;
		
		std::pair<char *, int> &pair = read_queue.front();
		
		int s = select(sockfd + 1, &set, NULL, NULL, &tv);
		if(s == 0)
		{
			return;
		}
		else
		if(s < 0)
		{
			throw TCPException("Error select");
		}
		
		int flags = fcntl(sockfd, F_GETFL, 0);
		fprintf(stdout,"read %d\n",pair.second);
		fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
		int r = ::read(sockfd, pair.first, pair.second);
		fcntl(sockfd, F_SETFL, flags);
		
		if(r < 0)
		{
			throw TCPException("Error reading data");
		}
		if(r == 0)
		{
			throw TCPAbortException();
		}
		if(r < pair.second)
		{
			pair.second -= r;
			pair.first += r;
		}
		else
		{
			read_queue.pop();
		}
	}
}

void TCPConnection::perform_write() throw(TCPException,TCPAbortException)
{
	if(sockfd < 0)
	{
		throw TCPException("Invalid connection");
	}
	
	while(write_queue.size())
	{
		struct timeval tv;
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sockfd, &set);
		
		tv.tv_sec = timeout/1000;
		tv.tv_usec = (timeout%1000)*1000;
		
		std::pair<const char *, int> &pair = write_queue.front();
		
		int s = select(sockfd + 1, NULL, &set, NULL, &tv);
		if(s == 0)
		{
			return;
		}
		else
		if(s < 0)
		{
			throw TCPException("Error select");
		}
		
		int flags = fcntl(sockfd, F_GETFL, 0);
		fprintf(stdout,"write %d\n",pair.second);
		fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
		int r = ::write(sockfd, pair.first, pair.second);
		fcntl(sockfd, F_SETFL, flags);
		
		if(r < 0)
		{
			throw TCPException("Error reading data");
		}
		if(r == 0)
		{
			throw TCPAbortException();
		}
		if(r < pair.second)
		{
			pair.second -= r;
			pair.first += r;
		}
		else
		{
			write_queue.pop();
		}
	}
}

void TCPConnection::abort_read()
{
	while(!read_queue.empty())
	{
		read_queue.pop();
	}
}

void TCPConnection::abort_write()
{
	while(!write_queue.empty())
	{
		write_queue.pop();
	}
}
