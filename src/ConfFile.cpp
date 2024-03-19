#include<webserv.hpp>
#include <poll.h>

ConfFile::ConfFile()
{
	
}

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
	if (pos == std::string::npos)
		return (i);
	while (pos != std::string::npos)
	{
		i++;
		pos = content.find("server ", pos + 1);
	}
	return (i);
}

void ConfFile::trimSpaces(std::string& str)
{
	str.erase(0, str.find_first_not_of(" \t"));

	str.erase(str.find_last_not_of(" \t") + 1);
}
	
void ConfFile::copyInfo(Cluster &cluster)
{
	cluster.getSocketVector() = this->getSocketVector();
	cluster.getServerVector() = this->getServerVector();
	cluster.getPollVector() = this->getPollVector();
}

void	ConfFile::parse_config(Cluster &cluster, char *file)
{
	std::ifstream in;
	std::string line;
	std::string content;
	std::string result;
	int servers;

	in.open(file, std::ios::in);
 	if (!in.is_open())
	{
		print_error("File could not be found or opened.\n");
		exit(EXIT_FAILURE);
	}
	while (std::getline(in, line))
		content += line + "\n";
	servers = countServers(content);
	if (servers == 0)
		throw std::runtime_error("at least one server needed.");
	std::istringstream iss(content);
	int i = 0;
	while (i < servers)
	{
		while (std::getline(iss, line, '\n'))
		{
			if (line.find("server ") == 0)
			{
				parse_element(content, i);
				i++;
			}
		}
	}
	copyInfo(cluster);
}

std::string ConfFile::findInfo(std::string line, std::string tofind)
{
	int tofindpos = line.find(tofind);
	int semicolonpos = line.find(";");
	std::string result;
	std::size_t firstNS;
	std::size_t lastNS;

	if (tofindpos < 0 || semicolonpos < 0)
		throw std::runtime_error("invalid configuration file.");
	result = line.substr(tofindpos + tofind.length(), semicolonpos);
	firstNS = result.find_first_not_of(" \t\n\r");
    lastNS = result.find_last_not_of(" \t\n\r;");
	result = result.substr(firstNS, lastNS - firstNS + 1);
	return (result);
}

void	ConfFile::findIp(Server& Serv, std::string newserv)
{
	std::size_t firstNonSpace;
	std::size_t lastNonSpace;
	std::string line;
	std::string res;
	std::string host;
	std::istringstream iss(newserv);
	size_t hasip;
	int pos;

	std::getline(iss, line, '\n');
	std::getline(iss, line, '\n');
	hasip = line.find(":");
	pos = line.find("listen ");
	if (hasip != std::string::npos)
	{
		res = line.substr(pos + 6, hasip - (pos + 6));
		firstNonSpace = res.find_first_not_of(" \t\n\r");
		lastNonSpace = res.find_last_not_of(" \t\n\r;");
		res = res.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
		host = res;
	}
	if (hasip == std::string::npos)
	{	
		host = "0.0.0.0";
		res = line.substr(pos + 6, line.find(";"));
	}
	else
		res = line.substr(hasip + 1, line.find(";"));
	firstNonSpace = res.find_first_not_of(" \t\n\r");
 	lastNonSpace = res.find_last_not_of(" \t\n\r;");
	res = res.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
	Serv.setHostPort(host, res);
}

void	ConfFile::parse_location(std::string line, Locations& loc)
{
	size_t pos;
	size_t fpos;
	std::string res;

	trimSpaces(line);

	if (line.find(" /") != std::string::npos && loc.getLocation().empty())
	{
		pos = line.find("/");
		fpos = line.find(" {");
		res = line.substr(pos, fpos - pos);
		trimSpaces(res);
		loc.setLocation(res);
	}
	else
	{
		fpos = line.find(";");
		if (fpos == std::string::npos)
			throw std::logic_error("falta coma");
		if (line.find("autoindex ") != std::string::npos)
		{
			pos = line.find("autoindex ");
			res = line.substr(pos + 10, fpos - pos);
			if (res == "on;" || res == "on")
				loc.setAutoindex(1);
			if (res == "off" || res == "off;")
				loc.setAutoindex(0);
		}
		else if (line.find("index ") != std::string::npos)
		{
			std::string index = findInfo(line, "index ");
			loc.setVIndex(splitString(index));
		}
		else if (line.find("allow_methods ") != std::string::npos)
		{
			int methods[3] = {0, 0, 0};
			pos = line.find("allow_methods ");
			res = (line.substr(pos + 14, fpos - pos));
			if (res.find("GET") != std::string::npos)
				methods[0] = 1;
			if (res.find("POST") != std::string::npos)
				methods[1] = 1;
			if (res.find("DELETE") != std::string::npos)
				methods[2] = 1;
			loc.setMethods(methods);
		}
		else if (line.find("return ") != std::string::npos)
		{
			pos = line.find("return ");
			res = line.substr(pos + 7, fpos - pos);
			loc.setRedirection(res.erase(res.size() - 1));
		}
		else if (line.find("root ") != std::string::npos)
		{
			pos = line.find("root ");
			res = line.substr(pos + 5, fpos - pos - 4);
			loc.setRoot(res.erase(res.size() - 1));
		}
		else if (line.find("cgi ") != std::string::npos)
		{
			pos = line.find("cgi ");
			res = line.substr(pos + 4, fpos - pos);
			pos = res.find(" /");
			fpos = res.find(";");
			std::string execute = res.substr(0, pos);
			trimSpaces(execute);
			std::string path = res.substr(pos, fpos);
			trimSpaces(path);
		}
		else
			throw std::logic_error(line + " => parametro incorrecto");
	}
}

std::vector<std::string> ConfFile::splitString(std::string& input)
{
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token)
	{
        tokens.push_back(token);
    } 
    return tokens;
}

int		ConfFile::parse_element(std::string &content, int i)
{
	size_t servpos = content.find("server ");
	static int id = 1;
	std::string newserv;
	std::string line;
	Server Serv;

	Serv.id = id;
	id++;
	if (servpos == std::string::npos)
		throw std::runtime_error("at least one server needed");
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
		else if (line.find("listen ") != std::string::npos)
			findIp(Serv, &line[line.find("listen ")]); 
		else if (line.find("server_name ") != std::string::npos)
			Serv.addVServerName(findInfo(line, "server_name"));
		else if (line.find("error_page ") != std::string::npos)
			Serv.setErrorPage(findInfo(line, "error_page "));
		else if (line.find("root ") != std::string::npos)
			Serv.setRoot(findInfo(line, "root "));
		else if (line.find("index ") != std::string::npos)
		{
			std::string index = findInfo(line, "index ");
			Serv.addVIndex(splitString(index));
		}
		else if (line.find("location ") != std::string::npos)
		{
			Locations loc;
			while (line.find("}") == std::string::npos)
			{
				parse_location(line, loc);
				std::getline(iss, line, '\n');
			}
			if (loc.getRoot().empty())
			{
				std::string res = loc.getLocation();
				res = res.erase(0, 1);
				loc.setAllUrl(Serv.getRoot() + res);
			}
			else
			{
				loc.setAllUrl(removeDoubleSlashes(loc.getRoot() + loc.getLocation()));
			}
			Serv.setLocation(loc);
		}
	}
	if (!Serv.host_port.size())
		Serv.setHostPort("0.0.0.0", "8080"); 
	this->serv_vec.push_back(Serv);
	return (0);
}

int	ConfFile::check_info()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{

	}
	return (0);
}


void	ConfFile::print_servers()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{
		std::cout << BGRED "Server " << i + 1 << ":" RESET << std::endl;
		this->serv_vec[i].printHostPort();
		this->serv_vec[i].printIpPort();
		this->serv_vec[i].printServer_Names();
		this->serv_vec[i].printRoot();
		this->serv_vec[i].printErrorPages();
		this->serv_vec[i].printindex();
		this->serv_vec[i].printLocations();
	}
}

void ConfFile::print_sockets()
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		cout << "Socket " << i + 1 <<  " with fd " << sock_vec[i].getFd()  << ". Is listener? " << sock_vec[i].listener << endl
		<< "IP: " << sock_vec[i].getIp() << " Port: " << sock_vec[i].getPort() << endl;  
	}
}

/*void	ConfFile::init_poll()
{
	size_t x = 0;
	int i = 0;
	for (x = 0; x < this->sock_vec.size(); x++)
	{
		i ++; 
	}
// 	this->poll_ptr = new struct pollfd[i];
//	this->fd_size = i;
//	this->fd_count = i;
	i = 0;
	for (x = 0; x < this->sock_vec.size(); x++)
	{
		this->poll_ptr[i].fd = this->sock_vec[x].s_fd; // asignamos el fd de cada socket a un poll
		this->poll_ptr[i].events = POLLIN; // los ponemos a que "nos avisen" al detectar conexiones entrantes
		i++;
	}
}*/
/*
static bool look_for_same(Socket &sock, std::vector<Socket>&sock_vec)
{
	for (std::vector<Socket>::iterator it = sock_vec.begin(); it != sock_vec.end(); it++)
	{
		si encontramos un socket creado con misma direccion:puerto, retornamos true y no se anade al vector
		if (it->getIp() == sock.getIp() && it->getPort() == sock.getPort())
			return (true);
		if (it->getPort() == sock.getPort() && it->getIp() == "0.0.0.0")
			return (true);
		if (it->getPort() == sock.getPort() && sock.getIp() == "0.0.0.0" && it->getIp() != "0.0.0.0")
		{
			sock_vec.erase(it);
			return (false);
		}
	}
	return (false);
}*/

void	ConfFile::create_sockets()
{	
	for (size_t i = 0; i < serv_vec.size(); i++)
	{
		for (size_t j = 0; j < serv_vec[i].host_port.size(); j++)
		{
			Socket s(serv_vec[i].host_port[j], &serv_vec[i]); // se crea socket, con host y puerto, se resuelve host a direccion ip 
												// con getaddr info en constructor de socket.
				
			if (!look_for_same(s, sock_vec))	// buscamos socket creado con misma direccion:puerto
			{
				sock_vec.push_back(s); // si devuelve falso, introducimos socket en vector para posterior bind.	
			}
		}
	}
}

