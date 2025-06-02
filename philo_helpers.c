/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:35 by bnafiai           #+#    #+#             */
/*   Updated: 2025/06/02 16:10:56 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	current_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	ft_atoi(char *s)
{
	int		i;
	long	res;
	int		sign;

	i = 0;
	res = 0;
	sign = 1;
	while (s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
		i++;
	if (s[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (s[i] == '+')
		i++;
	while (s[i] >= '0' && s[i] <= '9')
	{
		res = res * 10 + (s[i] - '0');
		i++;
	}
	return (res * sign);
}

int	ft_usleep(long millisec, t_data *data)
{
	long	start_time;

	start_time = current_time();
	while (current_time() - start_time < millisec)
	{
		pthread_mutex_lock(&data->death_mutex);
		if (data->someone_died)
		{
			pthread_mutex_unlock(&data->death_mutex);
			return (1);
		}
		pthread_mutex_unlock(&data->death_mutex);
		usleep(500);
	}
	return (0);
}

int	safe_print(t_philo *philo, char *message)
{
	pthread_mutex_lock(&philo->data->death_mutex);
	if (philo->data->someone_died == 1)
	{
		pthread_mutex_unlock(&philo->data->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->death_mutex);
	pthread_mutex_lock(&philo->data->print_mutex);
	printf("%ld %d %s\n", current_time() - philo->data->start_time,
		philo->id, message);
	pthread_mutex_unlock(&philo->data->print_mutex);
	return (0);
}

void	clean_up(t_philo *philo, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_philo)
	{
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&philo[i].mutex_meals);
		i++;
	}
	pthread_mutex_destroy(&data->death_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	free(data->forks);
	free(philo);
}
