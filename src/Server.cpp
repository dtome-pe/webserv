#include "../inc/Server.hpp"

Server::Server()
{
	sock_list	= NULL;
}

void	Server::start()
{
	for(t_sock *ptr = sock_list; ptr != NULL; ptr = ptr->next)
	{
		ptr->sock->start(); //inicializamos los datos del socket
	}
}