#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include<iostream>
#include<HeaderHTTP.hpp>

class Response
{
	public:
		Response();
		~Response();

		std::string	status_line;
		HeaderHTTP	headers;
		std::string body;
		std::string makeResponse(); // devuelve el texto con el formato completo
		void	setStatusLine(std::string _status_line);
		void	setHeader(std::string _header);
		void	setBody(std::string _body);
};

#endif
