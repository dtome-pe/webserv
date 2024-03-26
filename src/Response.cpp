#include <webserv.hpp>

Response::Response()
{	

}

Response::~Response()
{

}

void	Response::setBasicHeaders()
{
	setHeader("Server: Webserv");
	setHeader("Date: " + getCurrentTime());
}

void	Response::setResponse(int code, std::string arg, const Server *serv, const Location *loc)
{
	cout << "code: " << code << endl;
	setBasicHeaders();
	switch (code)
	{
		case 200:
		{
			setStatusLine("HTTP/1.1 200 OK");
			setHeader("Content-Length: " + getLengthAsString(arg));
			setBody(arg);
			break;
		}
		case 201:
		{
			setStatusLine("HTTP/1.1 201 Created");
			setHeader("Location: " + arg);
			setBody(arg);
			break;
		}
		case 204:
		{
			setStatusLine("HTTP/1.1 204 No Content");
			break;
		}
		case 301:
		{
			setStatusLine("HTTP/1.1 301 Moved Permanently");
			setHeader("Location: " + arg);
			setHeader("Connection: keep-alive");
			break;
		}
		case 302:
		{
			setStatusLine("HTTP/1.1 302 Found");
			setHeader("Location: " + arg);
			setHeader("Connection: keep-alive");
			break;
		}
		case 303:
		{
			setStatusLine("HTTP/1.1 303 See Other");
			setHeader("Location: " + arg);
			setHeader("Connection: keep-alive");
			break;
		}
		case 400:
		{
			setStatusLine("HTTP/1.1 400 Bad Request");
			makeDefault(400, *this, "/400.html", serv);
			break;
		}
		case 403:
		{
			setStatusLine("HTTP/1.1 403 Forbidden");
			makeDefault(403, *this, "/403.html", serv);
			break;
		}
		case 404:
		{
			setStatusLine("HTTP/1.1 404 Not Found");
			makeDefault(404, *this, "/404.html", serv);
			break;
		}
		case 405:
		{
			setStatusLine("HTTP/1.1 405 Method Not Allowed");
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
			setHeader(allow_header);
			makeDefault(405, *this, "/405.html", serv);
			break;
		}
		case 409:
		{
			setStatusLine("HTTP/1.1 409 Conflict");
			makeDefault(409, *this, "/409.html", serv);
			break;
		}
		case 500:
		{
			setStatusLine("HTTP/1.1 500 Internal Server Error");
			makeDefault(500, *this, "/500.html", serv);
			break;
		}
	}
}

std::string Response::makeResponse()
{
	return (this->status_line.line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Response::setStatusLine(std::string _status_line)
{
	this->status_line.line = _status_line + "\r\n";
	std::vector<std::string> split = HeaderHTTP::split(_status_line, " ");

	this->status_line.protocol = split[0];
	this->status_line.code = split[1];
	this->status_line.text = split[2];
}

void	Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Response::setBody(std::string _body)
{
	this->body = _body;
}
