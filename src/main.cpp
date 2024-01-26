#include <iostream>
#include <stdlib.h>
#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"

static void start_servers(t_data *data, t_s *list)
{
	t_s	*ptr;
	int	i = 0;

	ptr = list;
	while (ptr != NULL)
	{
		ptr->s->start();
		i++;
		ptr = ptr->next;
	}
	data->poll = new struct pollfd[i];
	ptr = list;
	i = 0;
	while (ptr != NULL)
	{
		data->poll[i].fd = ptr->s->s_fd;
		data->poll[i].events = POLLIN;
		ptr = ptr->next;
		i++;
	}
	data->fd_size = i;
	data->fd_count = i;
}

int	main(int argc, char *argv[])
{
	if (argc != 2)
	{
		print_error("Wrong number of arguments.");
		exit(EXIT_SUCCESS);
	}
	t_data	data;
	parse_config(argv[1], &data);
	print_servers(&data);
	start_servers(&data, data.s_list);
	poll_loop(&data, data.s_list);
}
