/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 15:36:25 by bnafiai           #+#    #+#             */
/*   Updated: 2025/06/01 15:27:25 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_last_meal(long time_of_last_meal, t_data *data, int philo_id)
{
	if (time_of_last_meal > data->time_to_die)
	{
		pthread_mutex_lock(&data->death_mutex);
		if (!data->someone_died)
		{
			printf("%ld %d died\n",
				current_time() - data->start_time, philo_id);
			data->someone_died = 1;
			pthread_mutex_unlock(&data->death_mutex);
			return (1);
		}
		pthread_mutex_unlock(&data->death_mutex);
	}
	return (0);
}

void	*check_for_death(t_data *data, int *all_ate_enough, int *total_meals)
{
	int		i;
	t_philo	*philo;
	long	time_of_last_meal;

	i = 0;
	while (i < data->nb_philo)
	{
		philo = &data->philo[i];
		pthread_mutex_lock(&philo->mutex_meals);
		time_of_last_meal = current_time() - philo->last_meal;
		*total_meals += philo->meals_eaten;
		if (data->nb_must_eat != -1 && philo->meals_eaten < data->nb_must_eat)
			*all_ate_enough = 0;
		pthread_mutex_unlock(&philo->mutex_meals);
		if (check_last_meal(time_of_last_meal, data, philo->id) == 1)
			return (NULL);
		i++;
	}
	return (NULL);
}

void	check_eat_required(t_data *data)
{
	pthread_mutex_lock(&data->death_mutex);
	if (!data->someone_died)
	{
		printf("%ld All philosophers have eaten enough times\n",
			current_time() - data->start_time);
		data->someone_died = 1;
	}
	pthread_mutex_unlock(&data->death_mutex);
}

void	*monitor_routine(void *args)
{
	t_data	*data;
	int		all_ate_enough;
	int		total_meals;

	data = (t_data *)args;
	while (1)
	{
		all_ate_enough = 1;
		total_meals = 0;
		if (check_for_death(data, &all_ate_enough, &total_meals) == NULL
			&& data->someone_died)
			return (NULL);
		if (data->nb_must_eat != -1 && (all_ate_enough
				|| total_meals >= (data->nb_philo * data->nb_must_eat)))
		{
			check_eat_required(data);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
