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

static void	add_pollfd(std::vector<pollfd> &pollVec, int fd)
{
	pollfd *node = new (pollfd);

	node->fd = fd;
	node->events = POLLIN;
	pollVec.push_back(*node);
}

static void	remove_pollfd(std::vector<pollfd> &pollVec, int fd)
{
	for (std::vector<pollfd>::iterator it = pollVec.begin(); it != pollVec.end(); it++)
	{
		if (it->fd == fd)
			pollVec.erase(it);
	}
}

/* static void	print_poll(std::vector<pollfd>&pollVec)
{
	for (unsigned int i = 0; i < pollVec.size(); i++)
	{
		std::cout << i << " socket fd: " << pollVec[i].fd << std::endl;
	}
} */

static int	receive_response(int new_socket, std::vector<unsigned char> *buff)
{
	int	result;

	result = recv(new_socket, buff->data(), 5000, 0);
	if (result != -1)
	{
		(*buff).resize(result);
		return (result);
	}
	else
		return (result);
}

static bool	checkIfListener(int poll_fd, std::vector<class Socket> sock_vec)
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (poll_fd == sock_vec[i].s_fd && sock_vec[i].listener)
			return (true);
	}
	return (false);
}

static Socket &findSocket(int socket_to_find, std::vector<Socket> sock_vec)
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (sock_vec[i].s_fd == socket_to_find)
			return (sock_vec[i]);
	}
	return (sock_vec[0]);
}

static Socket &findListener(std::vector<Socket> sock_vec, Socket &client)
{	
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (sock_vec[i].s_fd == client.pointingTo)
			return (sock_vec[i]);
	}
	return (sock_vec[0]);	
}

void	poll_loop(std::vector<pollfd> &pollVec, ConfFile &conf)
{	
	while (1)
	{
		//cout << "fd size is: " << fd_size << ". fd count is: " << fd_count << endl;
		int poll_count = poll(&pollVec[0], pollVec.size(), -1); // ?
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		for (unsigned int i = 0; i < pollVec.size(); i++) // buscamos que socket esta listo para recibir cliente
		{	
			//cout << "i es" << i << endl;
			//conf.print_sockets();
			if (pollVec[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{
				//cout << "i es" << i << endl;
				if (checkIfListener(pollVec[i].fd, conf.getSocketVector())) // si se trata de un listener, aceptamos conexion
				{	
					/*datos para nueva conexion*/
					int c_fd;
					struct sockaddr_in c_addr;
					socklen_t addrlen = sizeof (c_addr);
					/*datos para nueva conexion*/
					c_fd = accept(pollVec[i].fd, (struct sockaddr *) &c_addr, &addrlen); // el cliente acepta el socket
					if (c_fd == -1)
					{	
						print_error("client accept error");
						exit(1);
					}
					else
					{
						Socket *client = new Socket(ip_to_str(&c_addr) + port_to_str(&c_addr), NULL);
						client->pointTo(pollVec[i].fd);
						client->setClientFd(c_fd);
						conf.getSocketVector().push_back(*client);
						add_pollfd(pollVec, c_fd);
					}
				}
				else
				{
					int	nbytes;
					std::vector<unsigned char> buff(5000);
					nbytes = receive_response(pollVec[i].fd, &buff); // recibimos respuesta (recv)
					if (nbytes == 0)
					{
						cout << "connexion was closed with client" << endl;
						close(pollVec[i].fd);
						remove_pollfd(pollVec, i);
					}
					else if (nbytes == -1 && nbytes != EAGAIN)
					{
						print_error("recv error");
						close(pollVec[i].fd);
						remove_pollfd(pollVec, i);
					}
					else if (nbytes == EAGAIN)
						continue ;
					else
					{
						std::string	text;
						for (size_t i = 0; i < buff.size(); i++)
							text += buff[i];
						cout << text << endl;
						handle_client(pollVec[i].fd, conf, findListener(conf.getSocketVector(), findSocket(pollVec[i].fd, conf.getSocketVector())), text);
					}
				}
			}
		}
	}
}