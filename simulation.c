/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 15:47:34 by bnafiai           #+#    #+#             */
/*   Updated: 2025/06/03 23:45:27 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	left_right_lock(t_philo *philo, pthread_mutex_t *one, pthread_mutex_t *two)
{
	pthread_mutex_lock(one);
	if (safe_print(philo, "has taken a fork"))
	{
		pthread_mutex_unlock(one);
		return (1);
	}
	if (philo->data->nb_philo == 1)
	{
		ft_usleep(philo->data->time_to_die, philo->data);
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

int	handle_eating(t_philo *philo, pthread_mutex_t *one, pthread_mutex_t *two)
{
	pthread_mutex_lock(&philo->mutex_meals);
	philo->last_meal = current_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->mutex_meals);
	if (safe_print(philo, "is eating"))
	{
		pthread_mutex_unlock(one);
		pthread_mutex_unlock(two);
		return (1);
	}
	if (ft_usleep(philo->data->time_to_eat, philo->data))
	{
		pthread_mutex_unlock(one);
		pthread_mutex_unlock(two);
		return (1);
	}
	pthread_mutex_unlock(one);
	pthread_mutex_unlock(two);
	return (0);
}

void	simulation(t_philo *philo, pthread_mutex_t *one, pthread_mutex_t *two)
{
	pthread_mutex_lock(&philo->data->death_mutex);
	while (!philo->data->someone_died)
	{
		pthread_mutex_unlock(&philo->data->death_mutex);
		if (left_right_lock(philo, one, two))
			return ;
		if (handle_eating(philo, one, two))
			return ;
		if (safe_print(philo, "is sleeping"))
			return ;
		if (ft_usleep(philo->data->time_to_sleep, philo->data))
			return ;
		if (safe_print(philo, "is thinking"))
			return ;
		usleep(1000);
		pthread_mutex_lock(&philo->data->death_mutex);
	}
	pthread_mutex_unlock(&philo->data->death_mutex);
}
