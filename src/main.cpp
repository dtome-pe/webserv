#include <iostream>
#include <stdlib.h>
#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"

int	main(int argc, char *argv[])
{
	//cout << "webserv! argc es " << argc << endl;
	if (argc != 2)
	{
		print_error("Wrong number of arguments.");
		exit(EXIT_SUCCESS);
	}
	t_data	data;
	parse_config(argv[1], &data);
	print_servers(&data);
	//Server s;
	//s.start();
	//s.loop();
}
