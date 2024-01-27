#include "../inc/webserv.hpp"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fstream>

static int	parse_element(t_serv *list, std::string &line)
{
	if (!line.compare(0, 7, "\tlisten"))
	{
		if (parse_listen(&list->serv->sock_list, line))
			return (1);
	}
	return (0);
}

int	parse_config(const std::string& file, t_data *data)
{
	std::ifstream in;
	std::string line;
	(void)		data;

	in.open(file.c_str(), std::ios::in);
	if (!in.is_open())
	{
		print_error("File could not be found or opened.\n");
		exit(EXIT_FAILURE);
	}
	while (in.good())
	{
		getline(in, line);
		if (!line.compare("server:"))
			serv_back(&data->serv_list, serv_new());
		if (line.at(0) == '\t')
		{
			if (parse_element(serv_last(data->serv_list), line))
			{
				print_error("Wrong config file format.\n");
				exit(EXIT_FAILURE);
			}	
		}
	}
	return (0);	
}