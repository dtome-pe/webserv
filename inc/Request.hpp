#ifndef REQUEST_HPP
# define REQUEST_HPP

#include<HeaderHTTP.hpp>
#include "lib.hpp"

class Socket;
class Server;
class Location;
class Cluster;

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

		std::string			path;
		std::string			extension;

		std::string 		ip;
		std::string 		host;
		std::string 		port;

		std::string			cgiExtension;
		std::string			cgiBinary;

		std::string			uploadStore;

		bool				keepAlive;
		const Server		*serv;
		const Location		*loc;

		int					trailSlashRedir;

		bool				cgi;
		std::string			cgiOutput;

		Socket				&sock;
		Cluster				&cluster;

	public:
		Request(Cluster &cluster, std::string buff, const std::vector<class Server> &server, Socket &listener, Socket &client);
		~Request();
			
		void				setRequestLine(std::string _request_line);
		void				setHeader(std::string _header);
		void				setHeaders(HeaderHTTP headers);
		void				setBody(std::string _body);

		void				splitRequest(std::string buff);
		
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

		std::string			getPath();
		std::string			getExtension();
		
		bool				getKeepAlive();

		std::string			getUploadStore();
		std::string			getLocationDir();

		bool				getCgi();
		std::string			&getCgiOutput();

		Socket				&getSocket();
		Cluster				&getCluster();

		void				setCgiExtension(std::string &extension);
		void				setCgiBinary(std::string &binary);

		void 				setIpPortHost(Socket &listener);
		void				setTrailSlashRedir(bool redir);

		void				setServer(const std::vector<class Server> &server);
		void				setLocation(const Server *serv);

		void				setIp(std::string &ip);
		void				setHost(std::string &host);
		void				setPort(std::string &port);

		void				setPath(std::string path);
		void				setExtension(std::string extension);

		void				setUploadStore(std::string path);

		void				setCgi(bool cgi);
		void				setCgiOutput(std::string output);

		bool				good;

		void				printRequest();
		std::string 		makeRequest(); // devuelve el texto con el formato completo
};

#endif