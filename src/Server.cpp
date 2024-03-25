#include "../inc/Server.hpp"
#include <climits>

Server::Server()
{
	sock_list = NULL;
	active = 1;
	ip = "";
	server_name = "";
	error_page = "";
	body_size = 1;
	methods[0] = 1;
	methods[1] = 0;
	methods[2] = 0;
	methods[3] = 0;
}

void	Server::start()
{
	for(t_sock *ptr = sock_list; ptr != NULL; ptr = ptr->next)
	{
		ptr->sock->start(); //inicializamos los datos del socket
	}
}

std::string Server::getPort(const std::string &ip_port) const
{
	return (ip_port.substr(ip_port.find(":") + 1, ip_port.length()));
}

std::string Server::getIp(const std::string &ip_port) const
{
	return (ip_port.substr(0, ip_port.find(":")));
}

std::string Server::getServerName()
{
	return (this->server_name);
}

const std::map<int, std::string>& Server::getErrorPage() const
{
	return (this->merror_page);
}


const int *Server::getMethods() const
{
	return (this->methods);
}

std::string Server::getRoot() const
{
	return (this->root);
}

unsigned int Server::getMaxBodySize() const
{
	return (this->body_size);
}

void	Server::setPort(std::string port)
{
	this->port = port;
}

void	Server::setIp(std::string ip)
{
	this->ip = ip;
}

void	Server::setHostPort(std::string host, std::string port)
{

	host_port.push_back(host + ":" + port);
}

void	Server::setIpPort(std::string ip, std::string port)
{
	ip_port.push_back(ip + ":" + port);
}

void	Server::setServerName(std::string serverName)
{
	this->server_name = serverName;
}

void	Server::setMaxBodySize(std::string maxBody)
{
	std::string numStr;
	for (size_t i = 0; i < maxBody.length(); ++i)
	{
        if (std::isdigit(maxBody[i])) {
			numStr += maxBody[i];
		} else {
			if (maxBody[i] != 'm')
				throw std::runtime_error("the letter in max body must be m");
            break;
        }
    }
    unsigned long num = 0;
    std::istringstream(numStr) >> num;
	if (num > UINT_MAX)
		throw std::runtime_error("body size too large");
	else
		this->body_size = static_cast<unsigned int>(num);
}

void	Server::setErrorPage(std::string error_page)
{
	size_t pos = error_page.find(" ");
	int key = std::atoi(error_page.substr(0, pos).c_str());
	size_t fpos = error_page.find(";");
	error_page = error_page.substr(pos + 1, fpos);
	merror_page[key] = error_page;
}

void Server::setMethods(int met[4])
{
	for (int i = 0; i < 3; ++i) {
		methods[i] = met[i];
	}
}

void	Server::addVServerName(const std::string str)
{
	vserver_name.push_back(str);
}

void	Server::setRoot(std::string rt)
{
	root = rt;
}

const std::vector<std::string>& Server::getVServerName() const
{
	return (vserver_name);
}

void	Server::addVPort(std::string str)
{
	vports.push_back(str);
}

void	Server::addVIndex(std::vector<std::string> idxvec)
{
	vindex = idxvec;
}

const std::vector<std::string>& Server::getVIndex() const
{
	return (vindex);
}

std::vector<std::string>& Server::getVPort()
{
	return (vports);
}

void	Server::printHostPort()
{
	cout << GREEN "Host:Ports: " RESET << endl;
	for (size_t i = 0; i < host_port.size(); i++)
	{
		cout << host_port[i] << endl;
	}
}

void	Server::printIpPort()
{
	cout << GREEN "Ip:Ports: " RESET << endl;
	for (size_t i = 0; i < ip_port.size(); i++)
	{
		cout << ip_port[i] << endl;
	}
}

void	Server::printServer_Names()
{
	cout << GREEN "Server Names: " RESET << endl;
	for (size_t i = 0; i < vserver_name.size(); i++)
	{
		cout << vserver_name[i] << endl;
	}
}

void	Server::printRoot()
{
	std::cout << GREEN "Root: " RESET << this->root << std::endl;
}

void	Server::printindex()
{
	std::cout << GREEN "Index:" RESET << std::endl;
	for (size_t i = 0; i < vindex.size(); i++)
		std::cout << vindex[i] << std::endl;	
}

void	Server::printLocations()
{
	std::cout << std::endl << BOLD "LOCATIONS: " RESET << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		std::cout << std::endl << BOLD GREEN "Location " << i + 1 << ": " RESET << locations[i].getLocation() << std::endl;
		if (!locations[i].getRedirection().empty())
		{
			std::cout << GREEN "Return to: " RESET << "Number: " << locations[i].getRedirectionNumber() << " | Redirect: " << locations[i].getRedirection() << std::endl;
		}
		std::cout << GREEN "Autoindex: " RESET;
		if (locations[i].getAutoindex() == true)
			std::cout << "on" << std::endl;
		else
			std::cout << "off" << std::endl;
		std::cout << GREEN "Index:" RESET << std::endl;
		std::vector<std::string>aux = locations[i].getIndex();
		for (size_t i = 0; i < aux.size(); i++)
			std::cout << aux[i] << std::endl;
		std::cout << GREEN "Allow methods: " RESET;
		std::cout << "GET: ";
		if (locations[i].getMethods()[0] == 1)
			std::cout << "yes";
		else
			std::cout << "no";
		std::cout << " | POST: ";
		if (locations[i].getMethods()[1] == 1)
			std::cout << "yes";
		else
			std::cout << "no";
		std::cout << " | DELETE: ";
		if (locations[i].getMethods()[2] == 1)
			std::cout << "yes" << std::endl;
		else
			std::cout << "no" << std::endl;
		std::map<std::string, std::string> cgiMap = locations[i].getCGI();
        if (!cgiMap.empty())
		{
			std::cout << GREEN "CGI: " RESET << std::endl;
            std::map<std::string, std::string>::iterator it;
            for (it = cgiMap.begin(); it != cgiMap.end(); ++it)
            {
                std::cout << "Extension => " << it->first << " | Path => " << it->second << std::endl;
            }
        }
		std::cout << GREEN "Root: " RESET << locations[i].getRoot() << std::endl;
		std::cout << GREEN "All url: " RESET << locations[i].getAllUrl() << std::endl;
	}
}

void Server::printErrorPages()
{
	std::cout << GREEN "Error pages: " RESET << std::endl;
    std::map<int, std::string>::iterator it;
    for (it = merror_page.begin(); it != merror_page.end(); ++it) {
        std::cout << it->first << " => " << it->second << std::endl;
    }
}

void	Server::printBodySize()
{
	std::cout << GREEN "Max body size: " RESET << this->body_size << std::endl;
}

void	Server::setLocation(Locations& loc)
{
	locations.push_back(loc);
}

const std::vector<Locations>& Server::getLocations() const
{
        return (locations);
}
