#ifndef REQUEST_HPP
# define REQUEST_HPP

#include<iostream>
#include<HeaderHTTP.hpp>

typedef struct
{
	std::string	method;
	std::string target;
	std::string	version;
	std::string	line;
}	Request_line;

class Socket;

class Request
{
	public:
		Request(std::string buff, Socket &listener);
		~Request();
		Request_line	request_line;
		HeaderHTTP	headers;
		std::string body;
		std::string makeRequest(); // devuelve el texto con el formato completo
		void		setRequestLine(std::string _request_line);
		void		setHeader(std::string _header);
		void		setBody(std::string _body);

		void		splitRequest(std::string buff, Socket &listener);
		std::string getMethod();
		std::string getTarget();
		std::string getVersion();

		void setIpPortHost(Socket &listener);

		std::string ip;
		std::string host;
		std::string port;

		bool		good;

		void		printRequest();
};

#endif