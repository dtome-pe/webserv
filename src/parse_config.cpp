#include "../inc/webserv.hpp"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int	parse_config_aux(int argc, char *argv[])
{
	if (argc != 1 || argc != 2)
	{
		print_error("wrong number of arguments\n");
	}
	int	fd;
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		print_error(strerror(errno));
		exit(EXIT_FAILURE);
	}
	return (0);
}