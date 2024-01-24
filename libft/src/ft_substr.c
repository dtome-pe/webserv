/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theonewhoknew <theonewhoknew@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 10:07:12 by dtome-pe          #+#    #+#             */
/*   Updated: 2023/10/27 09:45:56 by theonewhokn      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"
#include <stdlib.h>
#include <stdio.h>

static size_t	ft_substrlen(const char *s, unsigned start, size_t len)
{
	unsigned int	c;

	c = 0;
	while (s[start] && c < len)
	{
		c++;
		start++;
	}
	return (c);
}

static char	*ft_null_string(void)
{
	char	*substr;

	substr = (char *)malloc(1);
	check_malloc_error(substr);
	substr[0] = '\0';
	return (substr);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char			*substr;
	unsigned long	i;
	size_t			substr_len;

	i = 0;
	if (start >= ft_strlen(s) || len == 0 || ft_strlen(s) == 0)
		return (ft_null_string());
	substr_len = ft_substrlen(s, start, len);
	substr = (char *)malloc(sizeof (char) * (substr_len + 1));
	check_malloc_error(substr);
	while (i < len && s[start])
		substr[i++] = s[start++];
	substr[i] = '\0';
	return (substr);
}
