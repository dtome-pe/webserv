#include "../inc/webserv.hpp"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fstream>

static void	add_server(t_data *data)
{
	data->s_list = s_new();
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
		if (line.compare("server:"))
			add_server(data);
		std::cout << line << std::endl;
	}

	return (0);	
}