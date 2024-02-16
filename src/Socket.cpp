#include<webserv.hpp>


Socket::Socket()
{
//	port = "8888";
}

Socket::~Socket()
{
}

void	Socket::start()
{
	get_addr_info(&s_addr, port.c_str()); // obtenemos datos
	s_fd = create_s(s_fd, s_addr); //creamos el fd del socket
	bind_s(s_fd, s_addr, ip);
	listen_s(s_fd);
	freeaddrinfo(s_addr);
}

/* void Socket::loop()
{
	int	new_socket;
	struct sockaddr_in c_addr;
    socklen_t c_addr_size = sizeof (c_addr);

	while (true)
	{	
		new_socket = accept(s_fd, (struct sockaddr *) &c_addr, &c_addr_size);
		if (new_socket < 0)
		{
            print_error(strerror(errno));
            exit(EXIT_FAILURE);
        }
		if (!fork())
		{	
			close(s_fd);
			handle_client(new_socket);
			close(new_socket);
			exit(0);
		}
		close(new_socket);
	}	
} */

std::string Socket::getPort() const
{
	return (this->port);
}

std::string Socket::getIp() const
{
	return (this->ip);
}

void	Socket::setPort(std::string port)
{
	this->port = port;
}

void	Socket::setIp(std::string ip)
{
	this->ip = ip;
}
