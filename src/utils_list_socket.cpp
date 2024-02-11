#include<webserv.hpp>
t_sock	*sock_new(std::string &port)
{
	t_sock	*node = new t_sock;

	node->sock = new Socket();
	node->sock->port = port;
	node->next = NULL;
	return (node);
}

int	sock_back(t_sock **lst, t_sock *new_sock)
{
	t_sock	*curr;

	if (!new_sock | !lst)
		return (1);
	if (*lst == NULL)
	{
		*lst = new_sock;
		return (0);
	}
	curr = sock_last(*lst);
	curr->next = new_sock;
	return (0);
}

t_sock	*sock_last(t_sock *lst)
{
	if (!lst)
		return (NULL);
	while (lst != NULL)
	{
		if (lst->next == NULL)
			return (lst);
		lst = lst->next;
	}
	return (lst);
}

int		sock_count(t_sock *lst)
{
	int i = 0;

	for (t_sock *ptr = lst; ptr != NULL; ptr = ptr->next)
	{
		i++;
	}
	return (i);
}
