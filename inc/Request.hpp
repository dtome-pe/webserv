#ifndef REQUEST_HPP
# define REQUEST_HPP

#include<HeaderHTTP.hpp>
#include "lib.hpp"

class Socket;
class Server;
class Location;

class Request
{	
	private:
		/*request elements*/	
		std::string			method;
		std::string 		target;
		std::string			version;
		std::string			request_line;
		HeaderHTTP			headers;
		std::string 		body;

		std::string 		ip;
		std::string 		host;
		std::string 		port;

		std::string			cgiExtension;
		std::string			cgiBinary;

		bool				keepAlive;
		const Server		*serv;
		const Location		*loc;

		int					trailSlashRedir;

	public:
		Request(std::string buff, Socket &listener);
		~Request();
			
		void				setRequestLine(std::string _request_line);
		void				setHeader(std::string _header);
		void				setBody(std::string _body);

		void				splitRequest(std::string buff, Socket &listener);
		
		std::string 		getMethod();
		std::string 		getTarget();
		std::string 		getVersion();
		std::string			getRequestLine();
		std::string			getHeader(std::string header);
		HeaderHTTP			getHeaders();
		std::string 		getBody();

		std::string			getIp();
		std::string			getHost();
		std::string			getPort();

		std::string 		getCgiExtension();
		std::string 		getCgiBinary();
		const Server		*getServer();
		const Location		*getLocation();
		bool				getTrailSlashRedir();

		void				setCgiExtension(std::string &extension);
		void				setCgiBinary(std::string &binary);

		void 				setIpPortHost(Socket &listener);
		void				setServer(const Server *serv);
		void				setLocation(const Location *loc);
		void				setTrailSlashRedir(bool redir);

		void				setIp(std::string &ip);
		void				setHost(std::string &host);
		void				setPort(std::string &port);

		bool				getKeepAlive();

		bool				good;

		void				printRequest();
		std::string 		makeRequest(); // devuelve el texto con el formato completo
};

#endif