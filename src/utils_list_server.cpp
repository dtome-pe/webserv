/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_list_server.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theonewhoknew <theonewhoknew@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 11:56:04 by theonewhokn       #+#    #+#             */
/*   Updated: 2024/01/25 14:50:21 by theonewhokn      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

t_s	*s_new()
{
	t_s	*node = new t_s;

	node->s = new Server();
	node->next = NULL;
	return (node);
}

int	s_back(t_s **lst, t_s *new_s)
{
	t_s	*curr;

	if (!new_s | !lst)
		return (1);
	if (*lst == NULL)
	{
		*lst = new_s;
		return (0);
	}
	curr = s_last(*lst);
	curr->next = new_s;
	return (0);
}

t_s	*s_last(t_s *lst)
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
