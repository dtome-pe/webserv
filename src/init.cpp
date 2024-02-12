#include<webserv.hpp>

void init_servers(t_data *data, t_serv *lst) // inicializamos los servers de la lista y los sockets de sus respectivas listas
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

void	init_poll(t_data *data, t_serv *lst)
{	
	int i = 0;
	for (t_serv *ptr = lst; ptr != NULL; ptr = ptr->next) // recorremos todos los servers
	{
		i += sock_count(ptr->serv->sock_list); // contamos todos los sockets de todos los servers
	}
 	data->poll = new struct pollfd[i]; // reservamos tantos polls como sockets haya
	i = 0;
	for (t_serv *ptr = lst; ptr != NULL; ptr = ptr->next) // recorremos todos los servers
	{
		for (t_sock *sock_ptr = ptr->serv->sock_list; sock_ptr != NULL; sock_ptr = sock_ptr->next) // recorremos todos los sockets de cada server
		{
			data->poll[i].fd = sock_ptr->sock->s_fd; // asignamos el fd de cada socket a un poll
			data->poll[i].events = POLLIN; // ?
			i++;
		}
	}
	data->fd_size = i;
	data->fd_count = i;
}

void	init_data(t_data *data)
{
	data->poll = NULL;
	data->serv_list = NULL;
}
