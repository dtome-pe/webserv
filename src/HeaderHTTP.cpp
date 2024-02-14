#include<webserv.hpp>

HeaderHTTP::HeaderHTTP()
{
	
}

HeaderHTTP::~HeaderHTTP()
{

}

void		HeaderHTTP::setHeader(std::string _header)
{
	this->header.push_back(_header);
}

std::string	HeaderHTTP::getHeader(std::string name)
{
	for (size_t i = 0; i < this->header.size(); i++)
		if (this->header[i].substr(0, this->header[i].find(':') - 1) == name)
			return (this->header[i]);
	return (NULL);
}
/* 
std::string	HeaderHTTP::getHeaderValue(std::string name)
{
	for (size_t i = 0; i < this->header.size(); i++)
		if (this->header[i].substr(0, this->header[i].find(':') - 1) == name)
			return (this->header[i].substr(this->header[i].find(':')
				, this->header[i].length()));
	return (NULL);
}
 */
std::string	HeaderHTTP::makeHeader()
{
	std::string text;
	for (size_t i = 0; i < this->header.size(); i++)
		text += this->header[i] + "\r\n";
	return (text);
}
