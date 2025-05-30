/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:27 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/30 02:43:19 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor_routine(void *args)
{
	t_data *data = (t_data *)args;
	int i;
	int all_ate_enough;
	int total_meals;

	while (1)
	{
		i = 0;
		all_ate_enough = 1;
		total_meals = 0;
		while (i < data->nb_philo)
		{
			t_philo *philo = &data->philo[i];
			long time_of_last_meal;
			pthread_mutex_lock(&philo->mutex_meals);
			time_of_last_meal = current_time() - philo->last_meal;
			total_meals += philo->meals_eaten;
			if (data->nb_must_eat != -1 && philo->meals_eaten < data->nb_must_eat)
				all_ate_enough = 0;
			pthread_mutex_unlock(&philo->mutex_meals);
			
			if (time_of_last_meal > data->time_to_die)
			{
				pthread_mutex_lock(&data->death_mutex);
				if (!data->someone_died)
				{
					printf("%ld %d died\n", current_time() - data->start_time, philo->id);
					data->someone_died = 1;
				}
				pthread_mutex_unlock(&data->death_mutex);
				return (NULL);
			}
			i++;
		}
		if (data->nb_must_eat != -1 && (all_ate_enough || total_meals >= (data->nb_philo * data->nb_must_eat)))
		{
			pthread_mutex_lock(&data->death_mutex);
			if (!data->someone_died)
			{
				printf("%ld All philosophers have eaten enough times\n", current_time() - data->start_time);
				data->someone_died = 1;
			}
			pthread_mutex_unlock(&data->death_mutex);
			return (NULL);
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
		pthread_mutex_lock(&philo->data->death_mutex);
		if (philo->data->someone_died)
		{
			pthread_mutex_unlock(&philo->data->death_mutex);
			return;
		}
		pthread_mutex_unlock(&philo->data->death_mutex);

		if (left_right_lock(philo, one, two))
			return;

		pthread_mutex_lock(&philo->mutex_meals);
		philo->last_meal = current_time();
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->mutex_meals);
		
		if (safe_print(philo, "is eating"))
		{
			pthread_mutex_unlock(one);
			pthread_mutex_unlock(two);
			return;
		}
		if (ft_usleep(philo->data->time_to_eat, philo->data))
		{
			pthread_mutex_unlock(one);
			pthread_mutex_unlock(two);
			return;
		}
		pthread_mutex_unlock(one);
		pthread_mutex_unlock(two);

		if (safe_print(philo, "is sleeping"))
			return;
		
		if (ft_usleep(philo->data->time_to_sleep, philo->data))
			return;
			
		if (safe_print(philo, "is thinking"))
			return;
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

	if (philo->id % 2)
		usleep(15000); 
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
	if (data->nb_philo <= 0 || data->time_to_die <= 0 ||
		data->time_to_eat <= 0 || data->time_to_sleep <= 0 ||
		(argv[5] != NULL && data->nb_must_eat <= 0))
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
