/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsin.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:57:20 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/23 15:57:20 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_for_max(char *str)
{
	long	res;

	res = ft_atoi(str);
	if (res > 2147483647)
	{
		printf ("the argument %s is more than int max\n", str);
		return (1);
	}
	return (0);
}

int	check_args(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str [i] == '-' && str[i + 1] != '\0')
		{
			printf("Error: argument %s in negatif \n", str);
			return (1);
		}
		if (str[i] == '+' && str[i + 1] != '\0')
			i++;
		if (str[i] < '0' || str[i] > '9')
		{
			printf("Error: argument %s is not a number \n", str);
			return (1);
		}
		i++;
	}
	if (check_for_max(str) == 1)
		return (1);
	return (0);
}
