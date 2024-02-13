#include<webserv.hpp>
#include <poll.h>

ConfFile::ConfFile(std::string _file)
{
	file = _file;
}

ConfFile::ConfFile()
{
	_ip = "";
	_port = "";
	_server_name = "";
}

ConfFile::~ConfFile()
{
}
/*
void ConfFile::parse_config() {
    std::ifstream in;
    std::string content;
    std::string line;
    in.open(file.c_str(), std::ios::in);
    if (!in.is_open()) {
        print_error("File could not be found or opened.\n");
        exit(EXIT_FAILURE);
    }
    while (std::getline(in, line)) {
        content += line + "\n";
    }
    in.close(); // Don't forget to close the file after reading

    int servers = countServers(content);
    std::vector<ConfFile> info(servers); // Use vector instead of array
    std::istringstream iss(content);
    int i = 0;
    while (std::getline(iss, line, '\n') && i < servers) {
        if (line.find("server ") != std::string::npos) {
            i++;
        }
        if (info[i]._port.empty()) {
            info[i].parse_element(line);
        }
        if (info[i]._server_name.empty()) {
            info[i]._server_name = findInfo(line, "server_name");
        }
    }
    for (i = 0; i < servers; ++i) {
        std::cout << "Server name: " << info[i]._server_name << std::endl;
        std::cout << "IP: " << info[i]._ip << std::endl;
        std::cout << "Port: " << info[i]._port << std::endl;
    }
}

int ConfFile::countServers(std::string content) {
    int i = 0;
    size_t pos = content.find("server ");
    while (pos != std::string::npos) {
        i++;
        pos = content.find("server ", pos + 1);
    }
    return i;
}

std::string ConfFile::findInfo(std::string line, std::string tofind) {
    size_t tofindpos = line.find(tofind);
    size_t semicolonpos = line.find(";");
    std::string result;
    if (tofindpos != std::string::npos && semicolonpos != std::string::npos) {
        result = line.substr(tofindpos + tofind.length(), semicolonpos - (tofindpos + tofind.length()));
        // Trim whitespace
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);
    }
    return result;
}

int ConfFile::parse_element(std::string &line) {
    size_t pos = line.find("listen");
    if (pos != std::string::npos) {
        size_t hasip = line.find(":");
        if (hasip != std::string::npos) {
            _ip = line.substr(pos + 6, hasip - (pos + 6));
        } else {
            _ip = ""; // Set to empty if no IP is specified
        }
        // Extract port
        size_t semicolonPos = line.find(";");
        if (semicolonPos != std::string::npos) {
            _port = line.substr(hasip + 1, semicolonPos - (hasip + 1));
            // Trim whitespace
            _port.erase(0, _port.find_first_not_of(" \t\n\r"));
            _port.erase(_port.find_last_not_of(" \t\n\r") + 1);
        }
        return 0;
    }
    return 1;
}


*//*
void ConfFile::parse_config()
{
	std::ifstream in;
	int servers;
	std::string content;
	std::string line;
	in.open(file.c_str(), std::ios::in);
	if (!in.is_open())
	{
		print_error("File could not be found or opened.\n");
		exit(EXIT_FAILURE);
	}
	while (std::getline(in, line))
		content += line + "\n";
	servers = countServers(content);
	ConfFile info[servers];
	int i = 0;
	std::istringstream iss(content);
	while (std::getline(iss, line, '\n') && i < servers)
	{
		if (line.find("server ") >= 0)
			i++;
		if (info[i]._port.empty())
			info[i].parse_element(line);
		if (info[i]._server_name.empty())
			info[i]._server_name = findInfo(line, "server_name");
	}
	i = 0;
	while (i < servers)
	{
		std::cout << "Server name: " << info[i]._server_name << std::endl;
		std::cout << "IP: " << info[i]._ip << std::endl;
		std::cout << "Port: " << info[i]._port << std::endl;
		i++;
	}
}*/

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


	in.open(file.c_str(), std::ios::in); 
 	if (!in.is_open())
	{
		print_error("File could not be found or opened.\n");
		exit(EXIT_FAILURE);
	}	
	while (std::getline(in, line))
		content += line + "\n";
	std::istringstream iss(content);
	int i = 0;
	while (std::getline(iss, line, '\n'))
	{
		if (line.find("server ") == 0)
		{
			Server S;
			this->serv_vec.push_back(S);
			i++;
		}
		parse_element(line);
	}
}

std::string ConfFile::findInfo(std::string line, std::string tofind)
{
	int tofindpos = line.find(tofind);
	int semicolonpos = line.find(";");
	std::string result;
	std::size_t firstNS;
	std::size_t lastNS;

	if (tofindpos < 0 || semicolonpos < 0)
		return ("");
	result = line.substr(tofindpos + tofind.length(), semicolonpos);
	firstNS = result.find_first_not_of(" \t\n\r");
    lastNS = result.find_last_not_of(" \t\n\r;");
	result = result.substr(firstNS, lastNS - firstNS + 1);
	return (result);
}

//Encuentra ip y server
int		ConfFile::parse_element(std::string &line)
{
	int pos = line.find("listen");
	int	semicolonPos = line.find(";");
	std::size_t firstNonSpace;
	std::size_t lastNonSpace;
	std::string port;
	Socket S;
	
	if (pos < 0 || semicolonPos < 0)
		return (1);
	int hasip = line.find(":");
	if (hasip > -1)
	{
		std::string ip = line.substr(pos + 6, hasip - (pos + 6));
		firstNonSpace = ip.find_first_not_of(" \t\n\r");
		lastNonSpace = ip.find_last_not_of(" \t\n\r;");
		ip = ip.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
		if (ip.find_first_not_of("0123456789.") != std::string::npos)
			return (1);
		this->_ip = ip;
		S.setIp(ip);
	}
	if (hasip == -1)
		port = line.substr(pos + 6, semicolonPos);
	else
		port = line.substr(hasip + 1, semicolonPos);
	firstNonSpace = port.find_first_not_of(" \t\n\r");
   	lastNonSpace = port.find_last_not_of(" \t\n\r;");
	port = port.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
	if (port.find_first_not_of("0123456789") != std::string::npos)
		return (1);
	this->_port = port;
	S.setPort(port);
	this->serv_vec.back().sock_vec.push_back(S);
//	S.setServerName(findInfo(line, "server_name"));
	//	this->serv_vec.back().sock_vec.push_back(S);
	return (0);
}
/*
void	ConfFile::print_servers()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{
		std::cout << "server " << i + 1 << ":" << std::endl;
		for (size_t j = 0; j < this->serv_vec[i].sock_vec.size(); j++)
		{
			std::cout << "Socket info:" << std::endl;
			std::cout << "Port: " << this->serv_vec[i].sock_vec[j].getPort() << std::endl;
			std::cout << "IP: ";
			if (!this->serv_vec[i].sock_vec[j].getIp().empty())
			   std::cout << this->serv_vec[i].sock_vec[j].getIp() << std::endl;
			else
				std::cout << "empty" << std::endl;
			std::cout << "Server name: " << this->serv_vec[i].sock_vec[j].getServerName() << std::endl;
		}
		std::cout << std::endl;
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
			this->poll_ptr[i].events = POLLIN; // ?
			i++;
		}
	}
}

*/
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
