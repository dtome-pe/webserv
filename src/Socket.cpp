#include<webserv.hpp>

Socket::Socket(std::string host_port)
{	
	host = host_port.substr(0, host_port.find(":"));
	port = host_port.substr(host_port.find(":") + 1, host_port.length());

	cout << "socket host is " << host << "socket port is " << port << endl;
	get_addr_info(&s_addr, host.c_str(), port.c_str()); // obtenemos datos y se resuelve dominio y se introduce ip y puerto.
}

void	Socket::start()
{
	s_fd = create_s(s_fd, s_addr); //creamos el fd del socket
	bind_s(s_fd, s_addr, ip);
	listen_s(s_fd);
	freeaddrinfo(s_addr);
}

std::string Socket::getPort() const
{
	return (this->port);
}

std::string Socket::getIp() const
{
	return (this->ip);
}

void	Socket::setHost(std::string host)
{
	this->host = host;
}

void	Socket::setPort(std::string port)
{
	this->port = port;
}

void	Socket::setIp(std::string ip)
{
	this->ip = ip;
}
