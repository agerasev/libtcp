#pragma once

#include "connection.hpp"
#include "exception.hpp"

class TCPClient
{
public:
	static TCPConnection *connect(const std::string &host, int port) throw(TCPException);
	static void disconect(TCPConnection *conn) throw(TCPException);
};
