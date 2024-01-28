#include "../inc/webserv.hpp"
#include <cstdlib>
#include "unistd.h"
/* 
static bool	check_if_listener(int poll_fd, t_serv *list)
{
	for (t_serv *ptr = list; ptr != NULL; ptr = ptr->next)
	{
		for (t_sock *sock_ptr = ptr->serv->sock_list; sock_ptr != NULL; sock_ptr = sock_ptr->next)
		{
			if (poll_fd == sock_ptr->sock->s_fd)
				return (true);
		}
	}
	return (false);
} */

/* static void	add_pollfd(struct pollfd *poll[], int new_fd, int *fd_count, int *fd_size)
{
	if (*fd_count == *fd_size)
		(*fd_size)++;
	*poll = (struct pollfd *)realloc(*poll, sizeof (**poll) * (*fd_size));
	
	(*poll)[*fd_count].fd = new_fd;
    (*poll)[*fd_count].events = POLLIN;
    (*fd_count)++;
}

static void	remove_pollfd(struct pollfd *poll[], int i, int *fd_count)
{
	poll[i] = poll[*fd_count - 1];

    (*fd_count)--;
} */

void	poll_loop(t_data *data) //, t_serv *list)
{	
	/*datos para nueva conexion*/
	int c_fd;
    struct sockaddr_in c_addr;
    socklen_t addrlen;
	/*datos para nueva conexion*/
	
	while (1)
	{
		int poll_count = poll(data->poll, data->fd_size, -1);
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < data->fd_size; i++) // buscamos que socket esta listo para recibir cliente
		{
			if (data->poll[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{
				//if (check_if_listener(data->poll[i].fd, list)) // comentamos de momento
				//{
					/*aceptamos nueva conexion, y gestionamos inmediatamente peticion cliente, ya que subject
					especifica solo UN POLL, para I/O entre cliente y servidor*/
				addrlen = sizeof (c_addr);
				c_fd = accept(data->poll[i].fd, (struct sockaddr *) &c_addr, &addrlen);
				if (c_fd == -1)
					print_error("client accept error");
				else
				{	
					handle_client(c_fd); // gestionamos cliente inmediatamente, efectuando I/O entre cliente y servidor en un poll
					close(c_fd);
				//	add_pollfd(&data->poll, c_fd, &data->fd_count, &data->fd_size);
				//	cout << "pollserver: new connection" << endl;
				}
			}
				//else /*si no es listener, es peticion de cliente*/
				//{
				//	if (handle_client(data->poll[i].fd))
				//	{	/*devuelve uno, conexion terminada o error en recv*/
				//		close(data->poll[i].fd);   /*cerramos fd y eliminamos de array pollfd*/
				//		remove_pollfd(&data->poll, i, &data->fd_count); 
				//	}
					/*si ha devuelto cero, peticion ha sido resuelta y la conexion sigue abierta*/
				//}
		}
	}
}