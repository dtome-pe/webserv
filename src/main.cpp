#include <iostream>
#include <stdlib.h>
#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"

static void start_servers(t_s *list)
{
	t_s	*ptr;

	ptr = list;
	while (ptr != NULL)
	{
		ptr->s->start();
		ptr = ptr->next;
	}
}

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
	start_servers(data.s_list);
	//s.loop();
}
