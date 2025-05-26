/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/26 16:18:12 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void *monitor(void *args)
{
	t_philo	*philo;
	long time_since_last_meal;
	philo = (t_philo *)args;
	while (1)
	{
		pthread_mutex_lock(&philo->mutex_meals);
		time_since_last_meal = current_time() - philo->last_meal;
		pthread_mutex_unlock(&philo->mutex_meals);
		pthread_mutex_lock(&philo->data->death_mutex);
		if (time_since_last_meal > philo->data->time_to_die && !philo->data->someone_died)
		{
			pthread_mutex_lock(&philo->data->print_mutex);
			printf("%ld %d died\n", current_time() - philo->data->start_time, philo->id);
			philo->data->someone_died = 1;
			pthread_mutex_unlock(&philo->data->print_mutex);
			pthread_mutex_unlock(&philo->data->death_mutex);
			break;
		}
		pthread_mutex_unlock(&philo->data->death_mutex);
		usleep(500);
	}
	return NULL;
}

void *philo_rotine(void *args)
{
	t_philo	*philo;
	philo = (t_philo *)args;
	pthread_t monitor_thread;
	
	pthread_mutex_lock(&philo->mutex_meals);
	philo->last_meal = current_time();
	pthread_mutex_unlock(&philo->mutex_meals);
	pthread_create(&monitor_thread, NULL, monitor, philo);
	pthread_detach(monitor_thread);
	
	if (philo->id % 2 == 0)
		usleep(1000);
		
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
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->right_fork);
			pthread_mutex_lock(philo->left_fork);
		}
		else
		{
			pthread_mutex_lock(philo->left_fork);
			pthread_mutex_lock(philo->right_fork);
		}
		
		pthread_mutex_lock(&philo->data->print_mutex);
		if (philo->data->someone_died)
		{
			pthread_mutex_unlock(&philo->data->print_mutex);
			pthread_mutex_unlock(philo->left_fork);
			pthread_mutex_unlock(philo->right_fork);
			break;
		}
		printf("%ld %d has taken a fork\n", current_time() - philo->data->start_time, philo->id);
		printf("%ld %d has taken a fork\n", current_time() - philo->data->start_time, philo->id);
		printf("%ld %d is eating\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
		pthread_mutex_lock(&philo->mutex_meals);
		philo->last_meal = current_time();
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->mutex_meals);
		ft_usleep(philo->data->time_to_eat * 1000, philo->data);
		
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		
		pthread_mutex_lock(&philo->data->print_mutex);
		if (philo->data->someone_died)
		{
			pthread_mutex_unlock(&philo->data->print_mutex);
			break;
		}
		printf("%ld %d is sleeping\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
		
		ft_usleep(philo->data->time_to_sleep * 1000, philo->data);
		
		pthread_mutex_lock(&philo->data->print_mutex);
		if (philo->data->someone_died)
		{
			pthread_mutex_unlock(&philo->data->print_mutex);
			break;
		}
		printf("%ld %d is thinking\n", current_time() - philo->data->start_time, philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
	}
	return (NULL);
}

static int	initalize_philo(t_data *data, char **argv)
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
	
	if (data->nb_philo <= 0 || data->time_to_die <= 0 || 
		data->time_to_eat <= 0 || data->time_to_sleep <= 0 ||
		(argv[5] != NULL && data->nb_must_eat <= 0))
	{
		printf("Error: Invalid argument values\n");
		return (1);
	}
	return (0);
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
		pthread_mutex_init(&philo[i].mutex_meals, NULL);
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
		if (initalize_philo(&data, argv))
			return (1);
		data.forks = malloc((sizeof(pthread_mutex_t) * data.nb_philo));
		if (!data.forks)
			return (1);
		data.someone_died = 0;
		pthread_mutex_init(&data.death_mutex, NULL);
		pthread_mutex_init(&data.print_mutex, NULL);
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
		data.start_time = current_time();
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
		i = 0;
		while (i < data.nb_philo)
		{
			pthread_mutex_destroy(&data.forks[i]);
			pthread_mutex_destroy(&philo_data[i].mutex_meals);
			i++;
		}
		free(data.forks);
		free(philo_data);
		pthread_mutex_destroy(&data.death_mutex);
		pthread_mutex_destroy(&data.print_mutex);
		return (0);
	}
	else
	{
		printf("Error: too much or too few arguments \n");
		return 1;
	}
	return 0;
}
