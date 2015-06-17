#include <stdio.h>
#include <string.h>
#include <tcp/client.hpp>

int main(int argc, char *argv)
{
	bool test = true;
	
	TCPConnection *conn = nullptr;
	
	try
	{
		conn = TCPClient::connect("192.168.0.65",8888);
	}
	catch(TCPException e)
	{
		fprintf(stderr,"Connection not created: %s\n",e.what());
		test = false;
	}
	
	if(conn != nullptr)
	{
		const char *msg = "client message";
		conn->queue_write(static_cast<int>(strlen(msg)));
		conn->queue_write(msg,strlen(msg));
		while(conn->write_size() != 0)
		{
			conn->perform_write();
		}
	}
	
	if(conn != nullptr)
	{
		TCPClient::disconect(conn);
	}
}
