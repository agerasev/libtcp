#pragma once

#include <exception>
#include <string>

class TCPException : public std::exception
{
private:
	std::string message;
	
public:
	TCPException(const std::string &msg)
	  : message(msg)
	{
		
	}
	
	std::string getMessage() const
	{
		return message;
	}
	
	virtual const char *what() const noexcept override
	{
		return message.data();
	}
};

class TCPAbortException : public TCPException
{
public:
	TCPAbortException()
	  : TCPException("Connection aborted")
	{
		
	}
};
