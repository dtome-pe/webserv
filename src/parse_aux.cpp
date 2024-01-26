#include "../inc/webserv.hpp"
#include <string>

int	parse_listen(t_s *list, std::string &line)
{	
	(void) list;
	size_t space_pos = line.find(' ');
	size_t semicolon_pos = line.find(';');
	std::string port = line.substr(space_pos + 1, semicolon_pos - space_pos - 1);
	if (port.find_first_not_of("0123456789") == std::string::npos)
	{
		list->s->port = port;
		return (0);
	}
	return (1);
}