#include<webserv.hpp>

Socket::Socket(std::string host_port, Server *s_ptr)
{	
	if (s_ptr) // listener
	{
		/*trocemos host y port para meterlas en funcion get_addr_info*/
		host = host_port.substr(0, host_port.find(":"));
		port = host_port.substr(host_port.find(":") + 1, host_port.length());

		get_addr_info(&s_addr, host.c_str(), port.c_str()); // obtenemos datos y se resuelve dominio y se introduce ip y puerto.
		struct sockaddr_in *addr = (sockaddr_in *) s_addr->ai_addr; // se castea para poder obtener ip
		ip = ip_to_str(addr);	// volcamos ip de network byte order a string, para luego chequear sockets duplicados
						// y posteriormente pasar informacion a server correspondiente
		serv = s_ptr;				//apuntamos a server correspondiente
		/*en este momento le seteamos en el vector ip_port al server que ha generado este socket, la ip 
		ya resuelta y puerto, para en find_serv_block poder buscar que server la toca gestionar
		la conexion recibida*/
		serv->setIpPort(ip, port);
		listener = 1;
	}
	else
	{
		host = host_port.substr(0, host_port.find(":"));
		listener = 0;
	}

}

void	Socket::start()
{
	s_fd = create_s(s_fd, s_addr, sock_addr, sock_addrlen); //creamos el fd del socket
	bind_s(s_fd, s_addr, ip); // bindeamos 
	listen_s(s_fd);
	freeaddrinfo(s_addr);
}

void	Socket::pointTo(int listener_fd)
{
	pointingTo = listener_fd;
}

void	Socket::setClientFd(int client_fd)
{
	s_fd = client_fd;
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
