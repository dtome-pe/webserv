#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include "Socket.hpp"
# include "Locations.hpp"
# include <poll.h>

class Server
{
	private:
		std::string ip;
		std::string port;
		std::string server_name;
		std::string error_page;
		std::string methods;
		std::string root;
		//vectores
		std::vector<std::string> vserver_name;
		std::vector<std::string> vports;
		std::vector<std::string> vindex;
		std::map<int, std::string> merror_page;
		std::vector<class Locations> locations;
	public:
		int		active;
		struct socket_list	*sock_list;
		std::vector<class Socket>	sock_vec;
		//vector host:puerto, (listen previo getaddrinfo resuelva dominio, si lo hay)
		std::vector<std::string> host_port;
		void	setHostPort(std::string host, std::string port); // funcion que anada un elemento en el vector de ip:puertos
		std::vector<std::string> ip_port;
		void	setIpPort(std::string ip, std::string port);

		Server();
		int id;
		void	start();
		//getters
		std::string	getPort(const std::string &ip_port) const;
		std::string	getIp(const std::string &ip_port) const;
		std::string getServerName();
		std::map<int, std::string>& getErrorPage();
		std::string getAllowMethods();
		std::string getRoot() const;
		//setters
		void	setPort(std::string port);
		void	setIp(std::string ip);
		void	setServerName(std::string serverName);
		void	setErrorPage(std::string error_page);
		void	setAllowMethods(std::string allow);
		void	setRoot(std::string rt);
		//setter/getter para vector server_name
		void	addVServerName(std::string str);
		const std::vector<std::string>& getVServerName() const;
		//setter/getter para vector ports
		void	addVPort(std::string str);
		std::vector<std::string>& getVPort();
		void	setLocation(Locations &loc);
		const std::vector<Locations>& getLocations() const;
		//setter/getter para vector index
		const std::vector<std::string>& getVIndex() const;
		void	addVIndex(std::vector<std::string> idxvec);
		void	printServer_Names();
		void	printLocations();
		void	printErrorPages();
		void	printindex();
		void	printRoot();
		void	printIpPort();
		void	printHostPort();


};

#endif
