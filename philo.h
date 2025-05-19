/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:31 by bnafiai           #+#    #+#             */
/*   Updated: 2025/05/19 16:25:32 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>

typedef struct s_data
{
	int	nb_philo;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	long	nb_must_eat;
}	t_data;

typedef struct s_philo
{
	int	id;
	int	meals_eaten;
	long	last_meal;

	pthread_t	thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_data	*data;
}	t_philo;


long ft_atoi(char *s);
#endif

