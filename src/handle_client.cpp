#include<webserv.hpp>

static	void print_request(std::vector<unsigned char> buff)
{
	for (std::vector<unsigned char>::const_iterator i = buff.begin(); i != buff.end(); ++i)
    {
		if (*i == '\r')
		{
			std::cout << "\\r";
			continue;
		}
		if (*i == '\n')
			std::cout << "\\n";
		std::cout << *i;
	}
	std::cout << std::endl;
}

static	int	receive_response(int new_socket, std::vector<unsigned char> *buff)
{
	int	result;

	result = recv(new_socket, buff->data(), 5000, 0);
	if (result != -1)
	{
		(*buff).resize(result);
		return (result);
	}
	else
		return (result);
}

int	handle_client(int new_socket)
{	
	int	nbytes;
	std::vector<unsigned char> buff(5000);
	nbytes = receive_response(new_socket, &buff); // recibimos respuesta (recv)
	if (nbytes <= 0)
	{
		/*error o conexion cerrada*/
		if (nbytes == 0)
			cout << "connexion was closed with client" << endl;
		if (nbytes == -1)
			print_error("recv error");
		return (1);
	}
	print_request(buff);
	std::string	text;
	for (size_t i = 0; i < buff.size(); i++)
		text += buff[i];
	Request	req(text);
/* 
	std::string	status_line = "HTTP/1.1 200 OK\r\n";
	std::string headers;
	std::string body = "Hi";
	
	std::string response = status_line + "Content-Length: 2\r\n" + "\r\n" + body;
 */
 	Response	msg;

	msg.setHeader("Server: apache");
	msg.setStatusLine("HTTP/1.1 200 OK");
	msg.setBody("Hi");
	msg.setHeader("Content-Length: 2");
	std::string response = msg.makeResponse(); // hacemos respuesta con los valores del clase Response

	send(new_socket, response.c_str(), response.length(), 0);

	return (0);
}