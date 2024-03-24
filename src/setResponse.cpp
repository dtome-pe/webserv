#include <webserv.hpp>

static	void	setBasicHeaders(Response &response)
{
	response.setHeader("Server: Webserv");
	response.setHeader("Date: " + getCurrentTime());
}

void	setResponse(int code, Response &response, std::string arg, const Server *serv, const Locations *loc)
{
	cout << "code: " << code << endl;
	setBasicHeaders(response);
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
		case 302:
		{
			response.setStatusLine("HTTP/1.1 302 Found");
			response.setHeader("Location: " + arg);
			response.setHeader("Connection: keep-alive");
			break;
		}
		case 303:
		{
			response.setStatusLine("HTTP/1.1 303 See Other");
			response.setHeader("Location: " + arg);
			response.setHeader("Connection: keep-alive");
			break;
		}
		case 400:
		{
			response.setStatusLine("HTTP/1.1 400 Bad Request");
			makeDefault(400, response, "/400.html", serv);
			break;
		}
		case 403:
		{
			response.setStatusLine("HTTP/1.1 403 Forbidden");
			makeDefault(403, response, "/403.html", serv);
			break;
		}
		case 404:
		{
			response.setStatusLine("HTTP/1.1 404 Not Found");
			makeDefault(404, response, "/404.html", serv);
			break;
		}
		case 405:
		{
			response.setStatusLine("HTTP/1.1 405 Method Not Allowed");
			std::string allow_header = "Allow: ";			/*allow header obligatorio en caso de mensaje 405 method not allowed
															donde se informan de metodos aceptados en URL*/
			if (loc)
			{
				if (loc->getMethods()[0] == 1)
					allow_header += "GET, ";
				if (loc->getMethods()[1] == 1)
					allow_header += "POST, ";
				if (loc->getMethods()[2] == 1)
					allow_header += "DELETE";
			}
			else
			{
				allow_header += "GET, ";
			}
			response.setHeader(allow_header);
			makeDefault(405, response, "/405.html", serv);
			break;
		}
	}
}