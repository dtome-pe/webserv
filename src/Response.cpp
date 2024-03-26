#include <webserv.hpp>

Response::Response()
{	
	body = "";
}

Response::~Response()
{

}

void	Response::setBasicHeaders(int code, Request &request)
{
	setHeader("Server: Webserv");
	setHeader("Date: " + getCurrentTime());
	setHeader("Connection: keep-alive");

	if (body != "")
		setHeader("Content-Length: " + getLengthAsString(body));


	if (code == 301 || code == 302 || code == 303)
	{	if (request.getTrailSlashRedir())
			setHeader("Location: " + request.getTarget() + "/");
		else
			setHeader("Location: " + request.getLocation()->getRedirection());
	}
}

void	Response::setResponse(int code, Request &request)
{
	cout << "code: " << code << endl;
	setBasicHeaders(code, request);
	switch (code)
	{
		case 200:
		{
			setStatusLine("HTTP/1.1 200 OK");
			break;
		}
		case 201:
		{
			setStatusLine("HTTP/1.1 201 Created");
			setHeader("Location: " + request.getTarget());
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
			break;
		}
		case 302:
		{
			setStatusLine("HTTP/1.1 302 Found");
			break;
		}
		case 303:
		{
			setStatusLine("HTTP/1.1 303 See Other");
			break;
		}
		case 400:
		{
			setStatusLine("HTTP/1.1 400 Bad Request");
			makeDefault(400, *this, "/400.html", request.getServer());
			break;
		}
		case 403:
		{
			setStatusLine("HTTP/1.1 403 Forbidden");
			makeDefault(403, *this, "/403.html", request.getServer());
			break;
		}
		case 404:
		{
			setStatusLine("HTTP/1.1 404 Not Found");
			makeDefault(404, *this, "/404.html", request.getServer());
			break;
		}
		case 405:
		{
			setStatusLine("HTTP/1.1 405 Method Not Allowed");
			std::string allow_header = "Allow: ";			/*allow header obligatorio en caso de mensaje 405 method not allowed
															donde se informan de metodos aceptados en URL*/
			if (request.getLocation())
			{
				if (request.getLocation()->getMethods()[0] == 1)
					allow_header += "GET, ";
				if (request.getLocation()->getMethods()[1] == 1)
					allow_header += "POST, ";
				if (request.getLocation()->getMethods()[2] == 1)
					allow_header += "DELETE";
				if (request.getLocation()->getMethods()[3] == 1)
					allow_header += "PUT";
			}
			else
			{
				allow_header += "GET, ";
			}
			setHeader(allow_header);
			makeDefault(405, *this, "/405.html", request.getServer());
			break;
		}
		case 409:
		{
			setStatusLine("HTTP/1.1 409 Conflict");
			makeDefault(409, *this, "/409.html", request.getServer());
			break;
		}
		case 500:
		{
			setStatusLine("HTTP/1.1 500 Internal Server Error");
			makeDefault(500, *this, "/500.html", request.getServer());
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
