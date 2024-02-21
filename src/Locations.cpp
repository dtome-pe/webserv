

#include "Locations.hpp"
#include <webserv.hpp>

Locations::Locations()
{
	autoindex = false;
	methods[0] = 1;
	methods[1] = 0;
	methods[2] = 0;
}

Locations::~Locations()
{

}

void	Locations::setAutoindex(bool autoin)
{
	autoindex = autoin;
}

void	Locations::setVIndex(std::vector<std::string> idx)
{
	index = idx;
}

void Locations::setMethods(int met[3])
{
	for (int i = 0; i < 3; ++i) {
		methods[i] = met[i];
	}
}

void	Locations::setLocation(std::string loc)
{
	location = loc;
}

void	Locations::setRedirection(std::string red)
{
	redirection = red;
}

void	Locations::setRoot(std::string rt)
{
	root = rt;
}

void	Locations::setAllUrl(std::string url)
{
	allurl = url;
}

bool	Locations::getAutoindex() const
{
	return (this->autoindex);
}

std::vector<std::string>	Locations::getIndex()
{
	return (index);
}

const int *Locations::getMethods() const
{
	return (this->methods);
}

const std::string Locations::getLocation() const
{
	return (this->location);
}

const std::string Locations::getRedirection() const
{
	return (this->redirection);
}

const std::string Locations::getRoot() const
{
	return (this->root);
}

std::string Locations::getAllUrl()
{
	return (this->allurl);
}
