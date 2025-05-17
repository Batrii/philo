/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/17 20:48:29 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

static int	check_for_max(char *str)
{
	long res;
	res = ft_atoi(str);
	if (res > 2147483647)
	{
		printf ("the argument %s is more that int max\n", str);
		return (1);
	}
	return (0);
}

static int	check_args(char *str)
{
	int i = 0;
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

int	main(int argc, char **argv)
{
	if (argc == 5 || argc == 6)
	{
		int i = 1;
		while (i < argc)
		{
			if (check_args(argv[i]))
				return (1);
			i++;
		}
	}
	else
	{
		printf("Error: too much or too few arguments \n");
		return 1;
	}
	return 0;
}
