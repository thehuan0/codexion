/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_simulation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:09:32 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:12:20 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->dongles_ready)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		pthread_cond_destroy(&sim->dongles[i].cond);
		heap_destroy(&sim->dongles[i].waiters);
		i++;
	}
	free(sim->dongles);
}

void	destroy_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->coders_ready)
	{
		pthread_mutex_destroy(&sim->coders[i].state_lock);
		i++;
	}
	free(sim->coders);
}

void	destroy_sync(t_simulation *sim)
{
	if (sim->sync_ready >= 1)
		pthread_mutex_destroy(&sim->log_lock);
	if (sim->sync_ready >= 2)
		pthread_mutex_destroy(&sim->stop_lock);
	if (sim->sync_ready >= 3)
		pthread_mutex_destroy(&sim->seq_lock);
}

void	cleanup_simulation(t_simulation *sim)
{
	destroy_dongles(sim);
	destroy_coders(sim);
	destroy_sync(sim);
	free(sim->threads);
}
