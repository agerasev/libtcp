#include <stdio.h>
#include <tcp/server.hpp>

int main(int argc, char *argv[])
{
	bool done = false;
	
	std::function<void(TCPConnection *)> handler = [&done](TCPConnection *conn)
	{
		try
		{
			while(!done)
			{
				int size;
				conn->queue_read(size);
				while(conn->read_size() != 0)
				{
					fprintf(stdout,"Perform read\n");
					conn->perform_read();
					if(done)
					{
						conn->abort_read();
						return;
					}
				}
				fprintf(stdout,"size = %d\n",size);
				
				char buffer[0x100];
				conn->queue_read(buffer,size);
				while(conn->read_size() != 0)
				{
					fprintf(stdout,"Perform read\n");
					conn->perform_read();
					if(done)
					{
						conn->abort_read();
						return;
					}
				}
				buffer[size] = '\0';
				fprintf(stdout,"%s\n",buffer);
			}
		}
		catch(TCPAbortException)
		{
			fprintf(stdout,"Connection aborted\n");
			return;
		}
	};
	
	std::function<void(const std::string &)> error_handler = [](const std::string &message)
	{
		fprintf(stderr,"%s\n",message.data());
	};

	TCPServer *server = new TCPServer(8888,handler,error_handler);
	
	getc(stdin);
	done = true;
	
	delete server;
}
