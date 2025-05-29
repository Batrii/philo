/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/29 15:27:58 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor_routine(void *args)
{
	t_data *data = (t_data *)args;
	int i;

	while (1)
	{
		i = 0;
		while (i < data->nb_philo)
		{
			t_philo *philo = &data->philo[i];
			long time_of_last_meal;
			pthread_mutex_lock(&philo->mutex_meals);
			time_of_last_meal = current_time() - philo->last_meal;
			pthread_mutex_unlock(&philo->mutex_meals);
			if (time_of_last_meal > data->time_to_die || (data->nb_must_eat != -1 && philo->meals_eaten > philo->data->nb_must_eat))
			{
				pthread_mutex_lock(&data->death_mutex);
				if (!data->someone_died)
				{
					printf("%ld %d is dead\n", current_time() - data->start_time, philo->id);
					data->someone_died = 1;
				}
				pthread_mutex_unlock(&data->death_mutex);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
int left_right_lock(t_philo *philo, pthread_mutex_t *one, pthread_mutex_t *two)
{
	pthread_mutex_lock(one);
	if (safe_print(philo, "has taken a fork"))
	{
		pthread_mutex_unlock(one);
		return (1);
	}
	pthread_mutex_lock(two);
	if (safe_print(philo, "has taken a fork"))
	{
		pthread_mutex_unlock(one);
		pthread_mutex_unlock(two);
		return (1);
	}
	return (0);
}
void simulation(t_philo *philo, pthread_mutex_t *one, pthread_mutex_t *two)
{
	while (1)
	{
		if (left_right_lock(philo, one, two) == 1)
			break;
		if (safe_print(philo, "is eating"))
		{
			pthread_mutex_unlock(one);
			pthread_mutex_unlock(two);
			break;
		}
		pthread_mutex_lock(&philo->mutex_meals);
		philo->last_meal = current_time();
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->mutex_meals);
		ft_usleep(philo->data->time_to_eat, philo->data);
		pthread_mutex_unlock(one);
		pthread_mutex_unlock(two);
		if (safe_print(philo, "is sleeping") == 1)
			break;
		ft_usleep(philo->data->time_to_sleep, philo->data);
		if (safe_print(philo, "is thinking") == 1)
			break;
	}
}
void *philo_rotine(void *args)
{
	t_philo			*philo;
	pthread_mutex_t	*number_one;
	pthread_mutex_t	*number_two;

	philo = (t_philo *)args;
	pthread_mutex_lock(&philo->mutex_meals);
	philo->last_meal = current_time();
	pthread_mutex_unlock(&philo->mutex_meals);

	if (philo->id % 2 == 0)
		usleep(500);
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

static int	initalize_data(t_data *data, char **argv)
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
	int	i;

	i = 0;
	while (i < data->nb_philo)
	{
		philo[i].id = i + 1;
		philo[i].meals_eaten = 0;
		philo[i].last_meal = data->start_time;
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
	pthread_t	monitor;
	if (argc == 5 || argc == 6)
	{
		int i = 1;
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
		if (data.nb_philo == 1)
		{
			printf("0 1 has taken a fork\n");
			usleep(data.time_to_die * 1000);
			printf("%ld 1 is dead\n", current_time() - data.start_time);
			free(data.forks);
			free(philo_data);
			return (0);
		}
		pthread_mutex_init(&data.death_mutex, NULL);
		pthread_mutex_init(&data.print_mutex, NULL);
		i = 0;
		while (i < data.nb_philo)
		{
			pthread_mutex_init(&data.forks[i], NULL);
			i++;
		}
		store_philo(philo_data, &data);
		i = 0;
		while (i < data.nb_philo)
		{
			pthread_create(&philo_data[i].thread, NULL, philo_rotine, &philo_data[i]);
			i++;
		}
		pthread_create(&monitor, NULL, monitor_routine, &data);
		pthread_join(monitor, NULL);
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
