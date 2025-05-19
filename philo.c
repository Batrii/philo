/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/19 16:43:27 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
// static void *test(void *data)
// {
// 	(void)data;
// 	printf("im the first thread executed \n");
// 	return (NULL);
// }
// static void *test2(void *data)
// {
// 	(void)data;
// 	printf("second thread running\n");
// 	return (NULL);
// }
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
static void	initalize_philo(t_data data, char **argv)
{
	data.nb_philo = ft_atoi(argv[1]);
	data.time_to_die = ft_atoi(argv[2]);
	data.time_to_eat = ft_atoi(argv[3]);
	data.time_to_sleep = ft_atoi(argv[4]);
	if (argv[5] != NULL)
		data.nb_must_eat = ft_atoi(argv[5]);
	else
		data.nb_must_eat = -1;
}
static void	store_philo(t_philo *philo, t_data *data)
{
	int i;
	i = 0;
	while (i < data->nb_philo)
	{
		philo[i].id = i + 1;
		philo[i].meals_eaten = 0;
		philo[i].left_fork = i;
		philo[i].right_fork = (i + 1) % data->nb_philo;
		philo[i].data = &data;
		i++;
	}
}
int	main(int argc, char **argv)
{
	// pthread_t	thread1, thread2;
	t_philo	*philo_data;
	t_data	data;
	if (argc == 5 || argc == 6)
	{
		int i = 1;
		while (i < argc)
		{
			if (check_args(argv[i]))
				return (1);
			i++;
		}
		initalize_philo(data, argv);
		philo_data = malloc((sizeof(t_philo) * data.nb_philo));
		if (!philo_data)
			return (1);
		store_philo(philo_data, &data);
	}
	else
	{
		printf("Error: too much or too few arguments \n");
		return 1;
	}
	return 0;
}
