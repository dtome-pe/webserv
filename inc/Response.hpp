#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include<iostream>
#include<HeaderHTTP.hpp>

typedef struct
{
	std::string	protocl;
	std::string code;
	std::string	text;
	std::string	whole_line;
}	Status_line;

class Response
{
	public:
		Response(Request &request);
		~Response();

		Status_line	status_line;
		HeaderHTTP	headers;
		std::string body;
		std::string makeResponse(); // devuelve el texto con el formato completo
		void 	do_get(Request &request);
		void 	do_post(Request &request);
		void 	do_delete(Request &request);
		void	setStatusLine(std::string _status_line);
		void	setHeader(std::string _header);
		void	setBody(std::string _body);
};

#endif
