/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:04 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:07 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_one_dongle(t_dongle *dongle, int capacity)
{
	dongle->taken = 0;
	dongle->holder_id = -1;
	dongle->available_at = 0;
	if (pthread_mutex_init(&dongle->lock, NULL) != 0)
		return (0);
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->lock);
		return (0);
	}
	if (!heap_init(&dongle->waiters, capacity))
	{
		pthread_cond_destroy(&dongle->cond);
		pthread_mutex_destroy(&dongle->lock);
		return (0);
	}
	return (1);
}

int	init_dongles(t_simulation *sim)
{
	int	i;
	int	n;

	n = sim->params.coders;
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (sim->dongles == NULL)
		return (0);
	i = 0;
	while (i < n)
	{
		if (!init_one_dongle(&sim->dongles[i], DONGLE_QUEUE_CAP))
			return (0);
		sim->dongles_ready++;
		i++;
	}
	return (1);
}
