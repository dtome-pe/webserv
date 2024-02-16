#include<webserv.hpp>
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

void	print_poll(pollfd *poll, int size)
{
	for (int i = 0; i < size; i++)
	{
		std::cout << i << " fd is " << poll[i].fd << std::endl;
		std::cout << i << " ev is " << poll[i].events << std::endl;
		std::cout << i << " rev is " << poll[i].revents << std::endl;
	}
}

void	poll_loop(pollfd *poll_ptr, int fd_size, ConfFile &conf)
{	
	/*datos para nueva conexion*/
	int c_fd;
    struct sockaddr_in c_addr;
    socklen_t addrlen;
	/*datos para nueva conexion*/
	
	while (1)
	{
		//print_poll(poll_ptr, fd_size);
		int poll_count = poll(poll_ptr, fd_size, -1); // ?
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < fd_size; i++) // buscamos que socket esta listo para recibir cliente
		{
			if (poll_ptr[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{	
				struct sockaddr_in sock_addr;
    			socklen_t sock_addrlen;
				sock_addrlen = sizeof(sock_addr);
				/*cuando hay un pollin (se ha detectado que un socket esta recibiendo una conexion entrante)
				cogemos informacion de ese socket, para coger ip y puerto, para comprobar posteriormente
				que server block necesita gestionar dicha peticion entrante*/
				if (getsockname(poll_ptr[i].fd, (struct sockaddr *)&sock_addr, &sock_addrlen) == -1)
				{
					print_error("error getting sock name");
					exit (1);
				}
				/*aceptamos nueva conexion, y gestionamos inmediatamente peticion cliente, ya que subject
				especifica solo UN POLL, para I/O entre cliente y servidor*/
				addrlen = sizeof (c_addr);
				c_fd = accept(poll_ptr[i].fd, (struct sockaddr *) &c_addr, &addrlen); // el cliente acepta el socket
				if (c_fd == -1)
					print_error("client accept error");
				else
				{
					handle_client(c_fd, conf, c_addr, sock_addr); // gestionamos cliente inmediatamente, efectuando I/O entre cliente y servidor en un poll
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