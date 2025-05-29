/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 15:47:10 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/29 16:19:00 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	clean_up(t_philo *philo, t_data data)
{
	int	i;

	i = 0;
	while (i < data.nb_philo)
	{
		pthread_mutex_destroy(&data.forks[i]);
		pthread_mutex_destroy(&philo[i].mutex_meals);
		i++;
	}
	pthread_mutex_destroy(&data.death_mutex);
	pthread_mutex_destroy(&data.print_mutex);
	free(data.forks);
	free(philo);
}

void	create_and_join(t_philo *philo, t_data *data)
{
	int			i;
	pthread_t	monitor;

	i = 0;
	while (i < data->nb_philo)
	{
		pthread_create(&philo[i].thread, NULL, philo_rotine, &philo[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_routine, data);
	pthread_join(monitor, NULL);
	i = 0;
	while (i < data->nb_philo)
	{
		pthread_join(philo[i].thread, NULL);
		i++;
	}
}

int	for_one(t_data *data)
{
	if (data->nb_philo == 1)
	{
		printf("0 1 has taken a fork\n");
		usleep(data->time_to_die * 1000);
		printf("%ld 1 is dead\n", current_time() - data->start_time);
		free(data->forks);
		free(data->philo);
		return (1);
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_philo	*philo_data;
	t_data	data;
	int		i;

	if (argc != 5 && argc != 6)
		return (printf("Error: too much or too few arguments \n"), 1);
	i = 1;
	while (i < argc)
	{
		if (check_args(argv[i]))
			return (1);
		i++;
	}
	if (initalize_data(&data, argv))
		return (1);
	data.forks = malloc((sizeof(pthread_mutex_t) * data.nb_philo));
	if (!data.forks)
		return (1);
	philo_data = malloc(sizeof(t_philo) * data.nb_philo);
	if (!philo_data)
		return (1);
	data.philo = philo_data;
	data.someone_died = 0;
	data.start_time = current_time();
	if (for_one(&data))
		return (0);
	pthread_mutex_init(&data.death_mutex, NULL);
	pthread_mutex_init(&data.print_mutex, NULL);
	i = 0;
	while (i < data.nb_philo)
	{
		pthread_mutex_init(&data.forks[i], NULL);
		i++;
	}
	store_philo(philo_data, &data);
	create_and_join(philo_data, &data);
	clean_up(philo_data, data);
	return (0);
}
