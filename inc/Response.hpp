#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

typedef struct
{
	std::string	protocol;
	std::string code;
	std::string	text;
	std::string	line;
}	Status_line;

class Server;
class Location;

class Response
{
	public:
		Response();
		~Response();

		Status_line	status_line;
		HeaderHTTP	headers;
		std::string body;
		std::string makeResponse(); // devuelve el texto con el formato completo

		void	handleRequest(Request &request, const Server *serv, const Location *loc);

		void	setStatusLine(std::string _status_line);
		void	setHeader(std::string _header);

		void	setResponse(int code, std::string arg, const Server *serv, const Location *loc);
		void	setBasicHeaders();
		void	setBody(std::string _body);
		void	setDelete(Request &request, std::string path);
};

#endif
