#include <cstddef>
#include <iostream>
#include <sys/socket.h>
#include <sstream>

void	*handle_client(void *arg)
{
	(void) arg;
	int	*new_socket;

	new_socket = (int *) arg;
	std::string	status_line = "HTTP/1.1 200 OK";
	std::string body = "Hi";
	
	std::string response = status_line + "\r\n" + "Content-Length: 2\r\n" + "\r\n" + body;

	send(*new_socket, response.c_str(), response.length(), 0);

	return (NULL);
}