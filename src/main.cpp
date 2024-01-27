#include <iostream>
#include <stdlib.h>
#include "../inc/webserv.hpp"
#include "../inc/Socket.hpp"

static void start_servers(t_data *data, t_serv *list)
{
	t_serv	*ptr;
	int	i = 0;
	(void) data;

	ptr = list;
	while (ptr != NULL)
	{
		ptr->serv->start();
		i++;
		ptr = ptr->next;
	}
/* 	data->poll = new struct pollfd[i];
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
	data->fd_count = i; */
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
	print_servers(data.serv_list);
	start_servers(&data, data.serv_list);
	//poll_loop(&data, data.serv_list);
}
