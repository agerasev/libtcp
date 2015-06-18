#pragma once

#include <queue>

#include "exception.hpp"

class TCPConnection
{
private:
	
	int sockfd;
	long timeout = 1000; // ms
	
	std::queue<std::pair<char *, int>> read_queue;
	std::queue<std::pair<const char *, int>> write_queue;
	
	void queue_read_data(char *data, int size);
	void queue_write_data(const char *data, int size);
	
public:
	TCPConnection(int fd);
	virtual ~TCPConnection();
	
	int get_fd() const;
	
	void set_timeout(long to);
	long get_timeout() const;
	
	template <typename T>
	void queue_read(T &data)
	{
		queue_read_data(reinterpret_cast<char *>(&data),sizeof(T));
	}
	
	template <typename T>
	void queue_read(T *data, int size)
	{
		queue_read_data(reinterpret_cast<char *>(data),size*sizeof(T));
	}
	
	template <typename T>
	void queue_write(const T &data)
	{
		queue_write_data(reinterpret_cast<const char *>(&data),sizeof(T));
	}
	
	template <typename T>
	void queue_write(const T *data, int size)
	{
		queue_write_data(reinterpret_cast<const char *>(data),size*sizeof(T));
	}
	
	int read_size() const;
	int write_size() const;
	
	void perform_read() throw(TCPException,TCPAbortException);
	void perform_write() throw(TCPException,TCPAbortException);
	
	void abort_read();
	void abort_write();
	
	int get_write_size() const;
	int get_read_size() const;
};
