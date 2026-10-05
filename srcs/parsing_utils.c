/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:53 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:56 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_digit_char(char c)
{
	return (c >= '0' && c <= '9');
}

int	is_valid_digit_string(const char *str)
{
	int	i;

	if (str == NULL || str[0] == '\0')
		return (0);
	i = 0;
	while (str[i] != '\0')
	{
		if (!is_digit_char(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	string_to_long(const char *str, long *out)
{
	long	result;
	int		i;

	result = 0;
	i = 0;
	while (str[i] != '\0')
	{
		if (result > (LONG_MAX - (str[i] - '0')) / 10)
			return (0);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	*out = result;
	return (1);
}

int	parse_numeric_field(const char *str, long *out)
{
	if (!is_valid_digit_string(str))
		return (0);
	if (!string_to_long(str, out))
		return (0);
	return (1);
}
