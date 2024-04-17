#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Server;
class Location;

class Response
{
	public:
		Response();
		~Response();

		std::string		protocol;
		std::string 	code;
		std::string		text;
		std::string		line;
		HeaderHTTP		headers;
		HeaderHTTP		cgiHeaders;
		std::string 	body;

		bool			waitingForBody;

		void			parseCgi(std::string text);

		std::string 	makeResponse(); // devuelve el texto con el formato completo
		
		void			makeDefault(int code, Request &request, Response &response, const std::string &file, const Server *serv);

		void			setStatusLine(std::string _status_line);
		void			setHeader(std::string _header);
		void			setCgiHeader(std::string _cgiHeader);
		void			setBody(std::string _body);

		int				getResponseCode(Request &request, const Server *serv, const Location *loc);
		void			setResponse(int code, Request &request);
		void			setBasicHeaders(int code, Request &request);

		std::string		getHeader(std::string _header);
		std::string		getCgiHeader(std::string _cgiHeader);

		void			setCode(std::string _code);
		std::string		getCode();
};

#endif
