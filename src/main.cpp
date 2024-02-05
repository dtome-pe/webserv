#include <iostream>
#include <stdlib.h>
#include "../inc/webserv.hpp"
#include "../inc/Socket.hpp"

static void init_servers(t_data *data, t_serv *lst) // inicializamos los servers de la lista y los sockets de sus respectivas listas
{
	t_serv	*ptr;
	int	i = 0;
	(void) data;

	ptr = lst;
	while (ptr != NULL)
	{
		ptr->serv->start(); //inicializamos los sockets de la lista de cada server
		i++;
		ptr = ptr->next;
	}
}

static void	init_poll(t_data *data, t_serv *lst)
{	
	int i = 0;
	for (t_serv *ptr = lst; ptr != NULL; ptr = ptr->next)
	{
		i += sock_count(ptr->serv->sock_list);
	}
 	data->poll = new struct pollfd[i];
	i = 0;
	for (t_serv *ptr = lst; ptr != NULL; ptr = ptr->next)
	{
		for (t_sock *sock_ptr = ptr->serv->sock_list; sock_ptr != NULL; sock_ptr = sock_ptr->next)
		{
			data->poll[i].fd =sock_ptr->sock->s_fd;
			data->poll[i].events = POLLIN;
			i++;
		}
	}
	data->fd_size = i;
	data->fd_count = i;
}

static void	init_data(t_data *data)
{
	data->poll = NULL;
	data->serv_list = NULL;
}

int	main(int argc, char *argv[])
{
	if (argc != 2)
	{
		print_error("Wrong number of arguments.");
		exit(EXIT_SUCCESS);
	}
	t_data	data;

	init_data(&data); // iniciamos lista de servidores y poll
	parse_config(argv[1], &data); //
	print_servers(data.serv_list); // printamos la info de los servers
	init_servers(&data, data.serv_list); // inicializamos los servers de la lista y los sockets de sus respectivas listas
	init_poll(&data, data.serv_list); // 
	poll_loop(&data);
}
