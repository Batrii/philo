/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnafiai <bnafiai@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:18:31 by bnafiai           #+#    #+#             */
/*   Updated: 2025/06/01 15:31:10 by bnafiai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

typedef struct s_data
{
	int				nb_philo;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	long			nb_must_eat;
	long			start_time;
	int				someone_died;
	pthread_mutex_t	*forks;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	print_mutex;
	struct s_philo	*philo;
}	t_data;

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long			last_meal;

	pthread_t		thread;
	pthread_mutex_t	mutex_meals;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	struct s_data	*data;
}	t_philo;

long	ft_atoi(char *s);
long	current_time(void);
int		check_args(char *str);
int		ft_usleep(long millisec, t_data *data);
int		safe_print(t_philo *philo, char *message);
int		initalize_data(t_data *data, char **argv);
void	store_philo(t_philo *philo, t_data *data);
void	*monitor_routine(void *args);
void	*philo_rotine(void *args);
#endif
