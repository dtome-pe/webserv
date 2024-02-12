#include<webserv.hpp>

void	print_error(const char *str)
{	
	cerr << str << endl;
}

void	print_servers(t_serv *list)
{	
	int	i = 1;
	for(t_serv	*ptr = list; ptr != NULL; ptr = ptr->next)
	{	
		cout << "server " << i << endl;
		for(t_sock	*sock_ptr = ptr->serv->sock_list; sock_ptr != NULL; sock_ptr = sock_ptr->next)
		{
			cout << "socket: " << endl;
			cout << sock_ptr->sock->getPort() << endl;
		}
		i++;
	}
}
