#ifndef REQUEST_HPP
# define REQUEST_HPP

#include<iostream>
#include<HeaderHTTP.hpp>

class Request
{
	public:
		Request(std::string buff);
		~Request();

		std::string	status_line;
		HeaderHTTP	headers;
		std::string body;
		std::string makeRequest(); // devuelve el texto con el formato completo
		void	setStatusLine(std::string _status_line);
		void	setHeader(std::string _header);
		void	setBody(std::string _body);
};

#endif