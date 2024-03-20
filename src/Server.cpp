#include "../inc/Server.hpp"

Server::Server()
{
	sock_list	= NULL;
	active = 1;
	ip = "";
	server_name = "";
	error_page = "";
	methods = "";
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

std::vector<std::map<int, std::string> >& Server::getErrorPage()
{
	return (verror_page);
}

std::string Server::getAllowMethods()
{
	return (this->methods);
}

std::string Server::getRoot() const
{
	return (this->root);
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

void	Server::setErrorPage(std::string error_page)
{
	size_t pos = error_page.find(" ");
	int key = std::atoi(error_page.substr(0, pos).c_str());
	size_t fpos = error_page.find(";");
	error_page = error_page.substr(pos + 1, fpos);
	std::map<int, std::string> errPage;
	errPage[key] = error_page;
	verror_page.push_back(errPage);
}

void	Server::setAllowMethods(std::string allow)
{
	this->methods = allow;
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
	cout << "Host:Ports: " << endl;
	for (size_t i = 0; i < host_port.size(); i++)
	{
		cout << host_port[i] << endl;
	}
}

void	Server::printIpPort()
{
	cout << "Ip:Ports: " << endl;
	for (size_t i = 0; i < ip_port.size(); i++)
	{
		cout << ip_port[i] << endl;
	}
}

void	Server::printServer_Names()
{
	cout << "Server Names: " << endl;
	for (size_t i = 0; i < vserver_name.size(); i++)
	{
		cout << vserver_name[i] << endl;
	}
}

void	Server::printRoot()
{
	std::cout << "Root: " << this->root << std::endl;
}

void	Server::printindex()
{
	std::cout << "Index:" << std::endl;
	for (size_t i = 0; i < vindex.size(); i++)
		std::cout << vindex[i] << std::endl;	
}

void	Server::printLocations()
{
	std::cout << std::endl << BOLD "LOCATIONS: " RESET << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		std::cout << BOLD "Location " << i + 1 << ": " RESET << locations[i].getLocation() << std::endl;
		if (!locations[i].getRedirection().empty())
			std::cout << "Redirection to: " << locations[i].getRedirection() << std::endl;
		std::cout << "Autoindex: ";
		if (locations[i].getAutoindex() == true)
			std::cout << "on" << std::endl;
		else
			std::cout << "off" << std::endl;
		std::cout << "Index:" << std::endl;
		std::vector<std::string>aux = locations[i].getIndex();
		for (size_t i = 0; i < aux.size(); i++)
			std::cout << aux[i] << std::endl;
		std::cout << "Allow methods: ";
		std::cout << "GET: ";
		if (locations[i].getMethods()[0] == 1)
			std::cout << "yes";
		else
			std::cout << "no";
		std::cout << " / POST: ";
		if (locations[i].getMethods()[1] == 1)
			std::cout << "yes";
		else
			std::cout << "no";
		std::cout << " / DELETE: ";
		if (locations[i].getMethods()[2] == 1)
			std::cout << "yes" << std::endl;
		else
			std::cout << "no" << std::endl;
		std::cout << "Root: " << locations[i].getRoot() << std::endl;
		std::cout << "All url: " << locations[i].getAllUrl() << std::endl;
	}
}

void Server::printErrorPages()
{
	std::cout << "Error pages: " << std::endl;
	std::vector<std::map<int, std::string> >& vecOfMaps = getErrorPage();
	for (size_t i = 0; i < vecOfMaps.size(); ++i)
	{
		std::map<int, std::string>::const_iterator iter;
		for (iter = vecOfMaps[i].begin(); iter != vecOfMaps[i].end(); ++iter)
		{
			std::cout << iter->first << " => " << iter->second << std::endl;
		}
		std::cout << std::endl;
	}
}

void	Server::setLocation(Locations& loc)
{
	locations.push_back(loc);
}

const std::vector<Locations>& Server::getLocations() const
{
        return (locations);
}
