/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/23 15:59:00 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void *monitor(void *args)
{
	t_philo	*philo;
	philo = (t_philo *)args;
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		if (current_time() - philo->last_meal > philo->data->time_to_die && !philo->data->someone_died)
		{
			pthread_mutex_lock(&philo->data->print_mutex);
			printf("%ld %d died\n", current_time() - philo->data->start_time, philo->id);
			philo->data->someone_died = 1;
			pthread_mutex_unlock(&philo->data->print_mutex);
		}
		pthread_mutex_unlock(&philo->data->death_mutex);
		usleep(100);
	}
}
void *philo_rotine(void *args)
{
	t_philo	*philo;
	philo = (t_philo *)args;
	pthread_t monitor_thread;
	
	philo->last_meal = current_time();
	pthread_create(&monitor_thread, NULL, monitor, philo);
	pthread_detach(monitor_thread);
	
	// if (philo->id % 2 == 0)
	// 	usleep(1000);
		
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		if (philo->data->someone_died || 
			(philo->data->nb_must_eat != -1 && philo->meals_eaten >= philo->data->nb_must_eat))
		{
			pthread_mutex_unlock(&philo->data->death_mutex);
			break;
		}
		pthread_mutex_unlock(&philo->data->death_mutex);
		
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(&philo->data->print_mutex);
		printf("%ld %d has taken a fork\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
		
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(&philo->data->print_mutex);
		printf("%ld %d has taken a fork\n", current_time() - philo->data->start_time, philo->id);
		printf("%ld %d is eating\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
		
		philo->last_meal = current_time();
		philo->meals_eaten++;
		usleep(philo->data->time_to_eat * 1000);
		
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		
		pthread_mutex_lock(&philo->data->print_mutex);
		printf("%ld %d is sleeping\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
		
		usleep(philo->data->time_to_sleep * 1000);
		
		pthread_mutex_lock(&philo->data->print_mutex);
		printf("%ld %d is thinking\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
	}
	return (NULL);
}

static void	initalize_philo(t_data *data, char **argv)
{
	data->nb_philo = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argv[5] != NULL)
		data->nb_must_eat = ft_atoi(argv[5]);
	else
		data->nb_must_eat = -1;
}
static void	store_philo(t_philo *philo, t_data *data)
{
	int i;
	i = 0;
	while (i < data->nb_philo)
	{
		philo[i].id = i + 1;
		philo[i].meals_eaten = 0;
		philo[i].last_meal = 0;
		philo[i].left_fork = &data->forks[i];
		philo[i].right_fork = &data->forks[(i + 1) % data->nb_philo];
		philo[i].data = data;
		i++;
	}
}
int	main(int argc, char **argv)
{
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
		initalize_philo(&data, argv);
		data.forks = malloc((sizeof(pthread_mutex_t) * data.nb_philo));
		if (!data.forks)
			return (1);
		data.someone_died = 0;
		pthread_mutex_init(&data.death_mutex, NULL);
		pthread_mutex_init(&data.print_mutex, NULL);
		data.start_time = current_time();
		i = 0;
		while (i < data.nb_philo)
		{
			pthread_mutex_init(&data.forks[i], NULL);
			i++;
		}
		philo_data = malloc(sizeof(t_philo) * data.nb_philo);
		if (!philo_data)
			return (1);
		store_philo(philo_data, &data);
		i = 0;
		while (i < data.nb_philo)
		{
			pthread_create(&philo_data[i].thread, NULL, philo_rotine, &philo_data[i]);
			i++;
		}
		i = 0;
		while (i < data.nb_philo)
		{
			pthread_join(philo_data[i].thread, NULL);
			i++;
		}
		return (0);
	}
	else
	{
		printf("Error: too much or too few arguments \n");
		return 1;
	}
	return 0;
}
