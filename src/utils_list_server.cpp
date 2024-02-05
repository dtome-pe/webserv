/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_list_server.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgravalo <jgravalo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 11:56:04 by theonewhokn       #+#    #+#             */
/*   Updated: 2024/02/05 14:55:45 by jgravalo         ###   ########.fr       */
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

int	serv_back(t_serv **lst, t_serv *new_serv)
{
	t_serv	*curr;

	if (!new_serv | !lst)
		return (1);
	if (*lst == NULL)
	{
		*lst = new_serv;
		return (0);
	}
	curr = serv_last(*lst);
	curr->next = new_serv;
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
