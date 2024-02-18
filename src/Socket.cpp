#include<webserv.hpp>

Socket::Socket(std::string host_port)
{	
	host = host_port.substr(0, host_port.find(":"));
	port = host_port.substr(host_port.find(":") + 1, host_port.length());

	get_addr_info(&s_addr, host.c_str(), port.c_str()); // obtenemos datos y se resuelve dominio y se introduce ip y puerto.

	std::ostringstream ipStream;
	struct sockaddr_in *addr = (sockaddr_in *) s_addr->ai_addr;
	uint32_t ipAddress = ntohl(addr->sin_addr.s_addr);

	ipStream << ((ipAddress >> 24) & 0xFF) << "."
             << ((ipAddress >> 16) & 0xFF) << "."
             << ((ipAddress >> 8) & 0xFF) << "."
             << (ipAddress & 0xFF);
	ip = ipStream.str();	// volcamos ip de network byte order a string.

	//cout << "socket host is " << host << " resolved to ip " << ip << " socket port is " << port << endl;
}

void	Socket::start()
{
	/*en este momento le seteamos en el vector ip_port al server que ha generado este socket, la ip 
	ya resuelta y puerto, para en find_serv_block poder buscar que server la toca gestionar
	la conexion recibida*/
	serv->setIpPort(ip, port); 
	s_fd = create_s(s_fd, s_addr); //creamos el fd del socket
	bind_s(s_fd, s_addr, ip); // bindeamos 
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
