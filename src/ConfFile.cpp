#include<webserv.hpp>
#include <poll.h>

ConfFile::ConfFile(std::string _file)
{
	file = _file;
}

ConfFile::~ConfFile()
{
}

int ConfFile::countServers(std::string content)
{
	int i = 0;
	size_t pos = content.find("server ");
	while (pos != std::string::npos)
	{
		i++;
		pos = content.find("server ", pos + 1);
	}
	return (i);
}


void	ConfFile::parse_config()
{
	std::ifstream in;
	std::string line;
	std::string content;
	std::string result;
	int servers;


	in.open(file.c_str(), std::ios::in);
 	if (!in.is_open())
	{
		print_error("File could not be found or opened.\n");
		exit(EXIT_FAILURE);
	}
	while (std::getline(in, line))
		content += line + "\n";
	servers = countServers(content);
	if (servers == 0)
	{
		std::cout << "Error in configuration file" << std::endl;
		exit(1);
	}
	std::istringstream iss(content);
	int i = 0;
	while (i < servers)
	{
		while (std::getline(iss, line, '\n'))
		{
			if (line.find("server ") == 0)
			{
			//	this->serv_vec.push_back(S);
				parse_element(content, i);
				i++;
			}
		}
	}
}

std::string ConfFile::findInfo(std::string line, std::string tofind, std::string found)
{
	int tofindpos = line.find(tofind);
	int semicolonpos = line.find(";");
	std::string result;
	std::size_t firstNS;
	std::size_t lastNS;

	if (!found.empty())
		return (found);
	if (tofindpos < 0 || semicolonpos < 0)
		return ("");
	result = line.substr(tofindpos + tofind.length(), semicolonpos);
	firstNS = result.find_first_not_of(" \t\n\r");
    lastNS = result.find_last_not_of(" \t\n\r;");
	result = result.substr(firstNS, lastNS - firstNS + 1);
	return (result);
}

void	ConfFile::findIp(Socket& S, std::string newserv)
{
	std::size_t firstNonSpace;
	std::size_t lastNonSpace;
	std::string line;
	std::string res;
	std::istringstream iss(newserv);
	int hasip;
	int pos;

	std::getline(iss, line, '\n');
	std::getline(iss, line, '\n');
	hasip = line.find(":");
	pos = line.find("listen ");
	if (hasip > -1)
	{
		res = line.substr(pos + 6, hasip - (pos + 6));
		firstNonSpace = res.find_first_not_of(" \t\n\r");
		lastNonSpace = res.find_last_not_of(" \t\n\r;");
		res = res.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
		S.setIp(res);
	}
	if (hasip == -1)
		res = line.substr(pos + 6, line.find(";"));
	else
		res = line.substr(hasip + 1, line.find(";"));
	firstNonSpace = res.find_first_not_of(" \t\n\r");
 	lastNonSpace = res.find_last_not_of(" \t\n\r;");
	res = res.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
	S.setPort(res);
}

int		ConfFile::parse_element(std::string &content, int i)
{
	int servpos = content.find("server ");
	std::string newserv;
	Socket S;
	std::string line;
	Server Serv;

	while (i > 0)
	{
		servpos = content.find("server ", servpos + 1);
		i--;
	}
	newserv = content.substr(servpos, content.length());
	std::istringstream iss(newserv);
	std::getline(iss, line, '\n');
	while (std::getline(iss, line, '\n'))
	{
		if (line.find("server ") == 0)
			break ;
		S.setIp(findInfo(line, "host ", S.getIp())); //quitar cuando se coja de server
		S.setPort(findInfo(line, "listen ", S.getPort())); //quitar cuando se coja de server
		if (line.find("host ") != std::string::npos)
			Serv.setIp(findInfo(line, "host ", Serv.getIp()));
		if (line.find("server_name ") != std::string::npos)
			Serv.addVServerName(findInfo(line, "server_name", ""));
		if (line.find("error_page ") != std::string::npos)
			Serv.setErrorPage(findInfo(line, "error_page ", Serv.getErrorPage()));
		if (line.find("listen ") != std::string::npos)
			Serv.addVPort(findInfo(line, "listen ", ""));
		if (line.find("allow_methods ") != std::string::npos)
			Serv.setAllowMethods(findInfo(line, "allow_methods ", Serv.getAllowMethods()));
	}
//	findIp(S, newserv); casos como 127.0.0.1:8001, falta gestionar para server	
	this->serv_vec.push_back(Serv);
	this->serv_vec.back().sock_vec.push_back(S);
	return (0);
}

int		ConfFile::check_info(Socket S)
{
	if (S.getPort().find_first_not_of("0123456789") != std::string::npos)
		return (1);
	else if (S.getIp().find_first_not_of("0123456789.") != std::string::npos)
		return (1);
	return (0);
}

void	ConfFile::print_servers()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{
		std::cout << BGRED "Server" << i + 1 << ":" RESET << std::endl;
		std::cout << "Port: ";
		std::vector<std::string>str = this->serv_vec[i].getVPort();
		for (std::vector<std::string>::iterator it = str.begin(); it != str.end(); it++)
			std::cout << *it << std::endl;
		std::cout << "Server name: ";
		str = this->serv_vec[i].getVServerName();
		for (std::vector<std::string>::iterator it = str.begin(); it != str.end(); it++)
			std::cout << *it << std::endl;
		std::cout << "IP: " << this->serv_vec[i].getIp() << std::endl;
	
		std::cout << "Error page: " << this->serv_vec[i].getErrorPage() << std::endl;
		std::cout << "Allow methods: " << this->serv_vec[i].getAllowMethods() << std::endl;
	}
}

void	ConfFile::init_serv()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{
		for (size_t j = 0; j < this->serv_vec[i].sock_vec.size(); j++)
		{
			this->serv_vec[i].sock_vec[j].start();
		}
	}
}

void	ConfFile::init_poll()
{
	size_t x = 0;
	int i = 0;
	for (x = 0; x < this->serv_vec.size(); x++) // recorremos todos los servers
	{
		i += (int)this->serv_vec[x].sock_vec.size(); // contamos todos los sockets de todos los servers
	}
 	this->poll_ptr = new struct pollfd[i]; // reservamos tantos polls como sockets haya
	this->fd_size = i;
	this->fd_count = i;
	i = 0;
	for (x = 0; x < this->serv_vec.size(); x++) // recorremos todos los servers
	{
		for (size_t j = 0; j < this->serv_vec[x].sock_vec.size(); j++) // recorremos todos los sockets de cada server
		{
			this->poll_ptr[i].fd = this->serv_vec[x].sock_vec[j].s_fd; // asignamos el fd de cada socket a un poll
			this->poll_ptr[i].events = POLLIN; // los ponemos a que "nos avisen" al detectar conexiones entrantes
			i++;
		}
	}
}

/*
void	ConfFile::poll_loop()
{
	//datos para nueva conexion//
	int c_fd;
    struct sockaddr_in c_addr;
    socklen_t addrlen;
	//datos para nueva conexion//

	while (1)
	{
		//this->poll_ptr = NULL;
		print_poll(poll_ptr, fd_size);
		int poll_count = poll(this->poll_ptr, this->fd_size, -1); // ?
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < this->fd_size; i++) // buscamos que socket esta listo para recibir cliente
		{
			if (this->poll_ptr[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{
				//if (check_if_listener(this->poll[i].fd, list)) // comentamos de momento
				//{
					//aceptamos nueva conexion, y gestionamos inmediatamente peticion cliente, ya que subject
					//especifica solo UN POLL, para I/O entre cliente y servidor
				addrlen = sizeof (c_addr);
				c_fd = accept(this->poll_ptr[i].fd, (struct sockaddr *) &c_addr, &addrlen); // el cliente acepta el socket

				if (c_fd == -1)
					print_error("client accept error");
				else
				{
					handle_client(c_fd); // gestionamos cliente inmediatamente, efectuando I/O entre cliente y servidor en un poll
					close(c_fd);
				//	add_pollfd(&this->poll_ptr, c_fd, &this->fd_count, &this->fd_size);
				//	cout << "pollserver: new connection" << endl;
				}
			}
				//else //si no es listener, es peticion de cliente
				//{
				//	if (handle_client(this->poll_ptr[i].fd))
				//	{	//devuelve uno, conexion terminada o error en recv
				//		close(this->poll_ptr[i].fd);   //cerramos fd y eliminamos de array pollfd
				//		remove_pollfd(&this->poll_ptr, i, &this->fd_count);
				//	}
					//si ha devuelto cero, peticion ha sido resuelta y la conexion sigue abierta
				//}
		}
	}
} */
