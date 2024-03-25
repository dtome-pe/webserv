#include <webserv.hpp>

Location::Location()
{
	autoindex = false;
	methods[0] = 1;
	methods[1] = 0;
	methods[2] = 0;
	methods[3] = 0;
}

Location::~Location()
{

}

void	Location::setAutoindex(bool autoin)
{
	autoindex = autoin;
}

void	Location::setVIndex(std::vector<std::string> idx)
{
	index = idx;
}

void Location::setMethods(int met[4])
{
	for (int i = 0; i < 4; ++i) {
		methods[i] = met[i];
	}
}

void	Location::setLocation(std::string loc)
{
	location = loc;
}

void	Location::setRedirection(std::string red)
{
	size_t pos = red.find(" ");
	size_t fpos = red.find(";");

	redirectNum = std::atoi(red.substr(0, pos).c_str());
	redirection = red.substr(pos + 1, fpos);
}

void	Location::setRoot(std::string rt)
{
	root = rt;
}

void	Location::setAllUrl(std::string url)
{
	allurl = url;
}

bool	Location::getAutoindex() const
{
	return (this->autoindex);
}

void	Location::setCGI(std::string ext, std::string path)
{
	cgi[ext] = path;
}

std::vector<std::string>	Location::getIndex() const
{
	return (index);
}

const int *Location::getMethods() const
{
	return (this->methods);
}

const std::string Location::getLocation() const
{
	return (this->location);
}

const std::string Location::getRedirection() const
{
	return (this->redirection);
}

int Location::getRedirectionNumber() const
{
	return (this->redirectNum);
}

const std::string Location::getRoot() const
{
	return (this->root);
}

std::string Location::getAllUrl()
{
	return (this->allurl);
}

std::map<std::string, std::string>Location::getCGI() const
{
	return (this->cgi);
}
