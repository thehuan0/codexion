/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:12:05 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:12:07 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.coders)
	{
		if (pthread_create(&sim->threads[i], NULL, coder_routine,
				&sim->coders[i]) != 0)
			return (0);
		sim->started++;
		i++;
	}
	return (1);
}

int	start_monitor(t_simulation *sim)
{
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
		return (0);
	sim->monitor_started = 1;
	return (1);
}

int	start_threads(t_simulation *sim)
{
	if (start_monitor(sim) && start_coders(sim))
		return (1);
	set_stop_flag(sim);
	join_threads(sim);
	return (0);
}

void	join_threads(t_simulation *sim)
{
	int	i;

	if (sim->monitor_started)
		pthread_join(sim->monitor, NULL);
	i = 0;
	while (i < sim->started)
	{
		pthread_join(sim->threads[i], NULL);
		i++;
	}
}
