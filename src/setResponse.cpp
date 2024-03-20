#include <webserv.hpp>

void	setResponse(int code, Response &response, std::string arg, const Server *serv, const Locations *loc)
{
	cout << "code: " << code << " arg: " << arg << endl;
	switch (code)
	{
		case 200:
		{
			response.setStatusLine("HTTP/1.1 200 OK");
			response.setHeader("Content-Length: " + getLengthAsString(arg));
			response.setBody(arg);
			break;
		}
		case 204:
		{
			response.setStatusLine("HTTP/1.1 204 No Content");
			break;
		}
		case 301:
		{
			response.setStatusLine("HTTP/1.1 301 Moved Permanently");
			response.setHeader("Location: " + arg);
			response.setHeader("Connection: keep-alive");
			break;
		}
		case 403:
		{
			response.setStatusLine("HTTP/1.1 403 Forbidden");
			makeDefault(response, "/403.html", serv);
			break;
		}
		case 404:
		{
			response.setStatusLine("HTTP/1.1 404 Not Found");
			makeDefault(response, "/404.html", serv);
			break;
		}
		case 405:
		{
			response.setStatusLine("HTTP/1.1 405 Method Not Allowed");
			std::string allow_header = "Allow: ";			/*allow header obligatorio en caso de mensaje 405 method not allowed
												donde se informan de metodos aceptados en URL*/
			if (loc->getMethods()[0] == 1)
				allow_header += "GET, ";
			if (loc->getMethods()[1] == 1)
				allow_header += "POST, ";
			if (loc->getMethods()[2] == 1)
				allow_header += "DELETE";
			response.setHeader(allow_header);
			makeDefault(response, "/405.html", serv);
			break;
		}
	}
}