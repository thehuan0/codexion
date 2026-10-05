/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:10 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:13 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_sync_primitives(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
		return (0);
	sim->sync_ready = 1;
	if (pthread_mutex_init(&sim->stop_lock, NULL) != 0)
		return (0);
	sim->sync_ready = 2;
	if (pthread_mutex_init(&sim->seq_lock, NULL) != 0)
		return (0);
	sim->sync_ready = 3;
	return (1);
}

int	init_simulation(t_simulation *sim, t_params *params)
{
	memset(sim, 0, sizeof(*sim));
	sim->params = *params;
	gettimeofday(&sim->start_time, NULL);
	if (!init_sync_primitives(sim))
		return (0);
	if (!init_dongles(sim))
		return (0);
	if (!init_coders(sim))
		return (0);
	return (1);
}
