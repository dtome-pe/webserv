#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include "Socket.hpp"
# include <poll.h>

class Server
{
	private:
		std::string ip;
		std::string port;
		std::string server_name;
		std::string error_page;
		std::string methods;
		//vectores
		std::vector<std::string> vserver_name;
		std::vector<std::string> vports;

		
	public:
		int		active;
		struct socket_list	*sock_list;
		std::vector<class Socket>	sock_vec;

		//vector host:puerto, (listen previo getaddrinfo resuelva dominio, si lo hay)
		std::vector<std::string> host_port;

		//vector ip:puerto (ip ya resuelta)
		std::vector<std::string> ip_port;

		Server();
		int id;
		void	start();
		//getters
		std::string	getPort();
		std::string	getIp();
		std::string getServerName();
		std::string getErrorPage();
		std::string getAllowMethods();
		//setters
		void	setPort(std::string port);
		void	setIp(std::string ip);
		void	setServerName(std::string serverName);
		void	setErrorPage(std::string errorPage);
		void	setAllowMethods(std::string allow);
		//setter/getter para vector server_name
		void	addVServerName(std::string str);
		const std::vector<std::string>& getVServerName() const;
		//setter/getter para vector ports
		void	addVPort(std::string str);
		std::vector<std::string>& getVPort();

		void	setHostPort(std::string host, std::string port); // funcion que actualice el map de ip:puertos

		void 	printHostPort();
		void	printServer_Names();

};

#endif
