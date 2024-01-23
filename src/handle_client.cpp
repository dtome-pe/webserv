#include <cstddef>
#include <iostream>
#include <sys/socket.h>
#include <sstream>
#include <vector>

static	void print_request(std::vector<unsigned char> buff)
{
	for (std::vector<unsigned char>::const_iterator i = buff.begin(); i != buff.end(); ++i)
    std::cout << *i;
	std::cout << std::endl;
}

static	int	receive_response(int new_socket, std::vector<unsigned char> *buff)
{
	int	result;

	result = recv(new_socket, buff->data(), 5000, 0);
	if (result != -1)
	{
		(*buff).resize(result);
		return (0);
	}
	else
		return (1);
}

int	handle_client(int new_socket)
{
	std::vector<unsigned char> buff(5000);
	receive_response(new_socket, &buff);
	print_request(buff);

	std::string	status_line = "HTTP/1.1 200 OK\r\n";
	std::string headers;
	std::string body = "Hi";
	
	std::string response = status_line + "Content-Length: 2\r\n" + "\r\n" + body;

	send(new_socket, response.c_str(), response.length(), 0);

	return (0);
}