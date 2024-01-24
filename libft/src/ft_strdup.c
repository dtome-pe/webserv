/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theonewhoknew <theonewhoknew@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:29:42 by dtome-pe          #+#    #+#             */
/*   Updated: 2023/10/27 09:45:24 by theonewhokn      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"
#include <stdlib.h>

char	*ft_strdup(char *str)
{
	char	*new;
	int		i;

	if (!str)
		return (NULL);
	i = 0;
	new = (char *)malloc(sizeof(char) * (ft_strlen(str) + 1));
	check_malloc_error(new);
	while (str[i])
	{
		new[i] = str[i];
		i++;
	}
	new[i] = '\0';
	return (new);
}
