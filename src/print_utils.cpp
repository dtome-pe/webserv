#include <errno.h>
#include <cstdio>
#include "../inc/webserv.hpp"

void	print_error(const char *str)
{	
	cerr << str << endl;
}

void	print_servers(t_data *data)
{
	t_s	*ptr;
	int	 i = 1;

	ptr = data->s_list;
	while (ptr != NULL)
	{	
		cout << "server " << i << endl;
		cout << "ip: " << ptr->s->ip << endl;
		cout << "port:  " << ptr->s->port << endl;
		ptr = ptr->next;
		i++;
	}
}