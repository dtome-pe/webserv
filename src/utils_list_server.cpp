/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_list_server.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theonewhoknew <theonewhoknew@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 11:56:04 by theonewhokn       #+#    #+#             */
/*   Updated: 2024/01/27 12:30:30 by theonewhokn      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "../inc/Server.hpp"

t_serv	*serv_new()
{
	t_serv	*node = new t_serv;

	node->serv = new Server();
	node->next = NULL;
	return (node);
}

int	serv_back(t_serv **lst, t_serv *new_sock)
{
	t_serv	*curr;

	if (!new_sock | !lst)
		return (1);
	if (*lst == NULL)
	{
		*lst = new_sock;
		return (0);
	}
	curr = serv_last(*lst);
	curr->next = new_sock;
	return (0);
}

t_serv	*serv_last(t_serv *lst)
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
