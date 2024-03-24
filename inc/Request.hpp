#ifndef REQUEST_HPP
# define REQUEST_HPP

#include<HeaderHTTP.hpp>
#include "lib.hpp"

class Socket;

class Request
{	
	private:
		/*request elements*/	
		std::string	method;
		std::string target;
		std::string	version;
		std::string	request_line;
		HeaderHTTP	headers;
		std::string body;

	public:
		Request(std::string buff, Socket &listener);
		~Request();
			
		void		setRequestLine(std::string _request_line);
		void		setHeader(std::string _header);
		void		setBody(std::string _body);

		void		splitRequest(std::string buff, Socket &listener);
		
		std::string getMethod();
		std::string getTarget();
		std::string getVersion();
		std::string	getRequestLine();
		std::string	getHeader(std::string header);
		HeaderHTTP	getHeaders();
		std::string getBody();

		void setIpPortHost(Socket &listener);

		std::string ip;
		std::string host;
		std::string port;

		bool		good;

		void		printRequest();
		std::string makeRequest(); // devuelve el texto con el formato completo
};

#endif