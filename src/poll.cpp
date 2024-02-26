#include<webserv.hpp>

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

static string &bounceBuff(string &text, vector<unsigned char>&buff)
{
	for (size_t i = 0; i < buff.size(); i++)
		text += buff[i];
	return (text);
}

static void	add_pollfd(std::vector<pollfd>&pollVec, std::vector<Socket>&sockVec, Socket &client, int fd)
{
	pollfd node;

	node.fd = fd;
	node.events = POLLIN;
	pollVec.push_back(node);
	sockVec.push_back(client);
}

static void	remove_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket>&sockVec, int fd)
{
	for (unsigned int i = 0; i < pollVec.size(); i++)
	{
		if (pollVec[i].fd == fd)
		{
			pollVec.erase(pollVec.begin() + i);
			break ;
		}
	}
	for (unsigned int i = 0; i < sockVec.size(); i++)
	{
		if (sockVec[i].s_fd == fd)
		{
			sockVec.erase(sockVec.begin() + i);
			break ;
		}
	}
}

void	poll_loop(std::vector<pollfd> &pollVec, ConfFile &conf)
{	
	//conf.print_sockets();
	while (1)
	{	
		//cout << "poll call" << endl;
		//conf.print_sockets();
		
		int poll_count = poll(&pollVec[0], pollVec.size(), -1); // ?
		//cout << "poll size is: " << pollVec.size() << ". fds to poll is (are): " << poll_count << endl;
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		unsigned int size = pollVec.size();
		for (unsigned int i = 0; i < size; i++) // buscamos que socket esta listo para recibir cliente
		{	
			//cout << "i es: " << i << endl;
			if (pollVec[i].revents == 0)
				continue ;
			if (pollVec[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{
				if (checkIfListener(pollVec[i].fd, conf.getSocketVector())) // si se trata de un listener, aceptamos conexion
				{
				//	cout << "se entra en pollin listener fd:" << pollVec[i].fd << " i es: " << i << endl;
					while (true) 
					{
						/*datos para nueva conexion*/
						int c_fd;
						struct sockaddr_in c_addr;
						socklen_t addrlen = sizeof (sockaddr_in);
						/*datos para nueva conexion*/
						c_fd = accept(pollVec[i].fd, (struct sockaddr *) &c_addr, &addrlen); // el cliente acepta el socket
						if (c_fd == -1)
						{	
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								cout << "accept: " << strerror(errno) << endl;
								exit(1);
							}
							else
								break ;
						}
						else
						{
							//cout << "accepted fd is: " << c_fd << endl;
							Socket client(ip_to_str(&c_addr) + port_to_str(&c_addr), NULL);
							client.pointTo(pollVec[i].fd);
							client.setClientFd(c_fd);
							client.setNonBlocking(c_fd);
							add_pollfd(pollVec, conf.getSocketVector(), client, c_fd);
							//cout << "client added to pollfd. now socket configuration is" << endl;
						}
					}
				}
				else
				{	
					//cout << "se entra en pollin cliente fd:" << pollVec[i].fd << " i es" << i << endl;
					string	text = "";
					int		nbytes;
					while (true)
					{
						std::vector<unsigned char> buff(5000);
						nbytes = receive_response(pollVec[i].fd, &buff); // recibimos respuesta (recv)
						//cout << "nbytes result: " << nbytes << endl;
						if (nbytes == -1)
						{	
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								close(pollVec[i].fd);
								//cout << "recv: " << strerror(errno) << endl;
								remove_pollfd(pollVec, conf.getSocketVector(), pollVec[i].fd);
								break ;
							}
							//cout << "EAGAIN / EWOULDBLOCK" << endl;
							break ;	
						}
						else if (nbytes == 0)
						{	
							close(pollVec[i].fd);
						//	cout << "connexion with fd: " << pollVec[i].fd << " was closed with client. socket config is: " << endl;
							remove_pollfd(pollVec, conf.getSocketVector(), pollVec[i].fd);
							//conf.print_sockets();
							break ;
						}
						bounceBuff(text, buff);
					}
					if (text != "")
					{
						handle_client(pollVec[i].fd, conf, findListener(conf.getSocketVector(), 
							findSocket(pollVec[i].fd, conf.getSocketVector())),text);
					}
				}
			}
		}
	}
}