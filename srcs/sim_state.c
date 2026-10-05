/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:12:01 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:12:03 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	current_ms(t_simulation *sim)
{
	struct timeval	now;
	long			elapsed_us;

	gettimeofday(&now, NULL);
	elapsed_us = (now.tv_sec - sim->start_time.tv_sec) * 1000000L;
	elapsed_us += now.tv_usec - sim->start_time.tv_usec;
	return (elapsed_us / 1000);
}

long	sim_start_epoch_us(t_simulation *sim)
{
	return (sim->start_time.tv_sec * 1000000L + sim->start_time.tv_usec);
}

long	next_sequence(t_simulation *sim)
{
	long	value;

	pthread_mutex_lock(&sim->seq_lock);
	sim->request_seq++;
	value = sim->request_seq;
	pthread_mutex_unlock(&sim->seq_lock);
	return (value);
}

int	simulation_stopped(t_simulation *sim)
{
	int	value;

	pthread_mutex_lock(&sim->stop_lock);
	value = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	return (value);
}

void	set_stop_flag(t_simulation *sim)
{
	int	i;

	pthread_mutex_lock(&sim->stop_lock);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_lock);
	i = 0;
	while (i < sim->params.coders)
	{
		pthread_mutex_lock(&sim->dongles[i].lock);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].lock);
		i++;
	}
}
