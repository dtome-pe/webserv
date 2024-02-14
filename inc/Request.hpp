#ifndef REQUEST_HPP
# define REQUEST_HPP

#include<iostream>
#include<HeaderHTTP.hpp>

typedef struct
{
	std::string	instruction;
	std::string	model;
}	Request_line;

class Request
{
	public:
		Request(std::string buff);
		~Request();
		Request_line	Request_line;
		std::string	status_line;
		HeaderHTTP	headers;
		std::string body;
		std::string makeRequest(); // devuelve el texto con el formato completo
		void	setStatusLine(std::string _status_line);
		void	setHeader(std::string _header);
		void	setBody(std::string _body);
		void	printLine(std::string line);
		void	splitRequest(std::string buff);

};

#endif