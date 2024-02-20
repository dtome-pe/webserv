

#include "Locations.hpp"
#include <webserv.hpp>

Locations::Locations()
{
	autoindex = false;
	index = "";
	methods[0] = 0;
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

void	Locations::setIndex(std::string idx)
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

bool	Locations::getAutoindex()
{
	return (this->autoindex);
}

std::string	Locations::getIndex()
{
	return (this->index);
}

int *Locations::getMethods()
{
	return (this->methods);
}

const std::string Locations::getLocation() const
{
	return (this->location);
}

std::string Locations::getRedirection()
{
	return (this->redirection);
}