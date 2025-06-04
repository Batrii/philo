/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/06/04 18:20:48 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philo_rotine(void *args)
{
	t_philo			*philo;
	pthread_mutex_t	*number_one;
	pthread_mutex_t	*number_two;

	philo = (t_philo *)args;
	pthread_mutex_lock(&philo->mutex_meals);
	philo->last_meal = current_time();
	pthread_mutex_unlock(&philo->mutex_meals);
	if (philo->id % 2)
		usleep(1000);
	if (philo->id % 2 == 0)
	{
		number_one = philo->left_fork;
		number_two = philo->right_fork;
	}
	else
	{
		number_one = philo->right_fork;
		number_two = philo->left_fork;
	}
	simulation(philo, number_one, number_two);
	return (NULL);
}

int	initalize_data(t_data *data, char **argv)
{
	data->nb_philo = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argv[5] != NULL)
		data->nb_must_eat = ft_atoi(argv[5]);
	else
		data->nb_must_eat = -1;
	data->someone_died = 0;
	if ((data->nb_philo <= 0 || data->nb_philo > 200) || data->time_to_die <= 0
		|| data->time_to_eat <= 0 || data->time_to_sleep <= 0
		|| (argv[5] != NULL && data->nb_must_eat <= 0))
	{
		printf("Error: Invalid argument values\n");
		return (1);
	}
	return (0);
}

void	store_philo(t_philo *philo, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_philo)
	{
		philo[i].id = i + 1;
		philo[i].meals_eaten = 0;
		philo[i].last_meal = current_time();
		philo[i].left_fork = &data->forks[i];
		philo[i].right_fork = &data->forks[(i + 1) % data->nb_philo];
		pthread_mutex_init(&philo[i].mutex_meals, NULL);
		philo[i].data = data;
		i++;
	}
}
