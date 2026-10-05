/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_release.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:30 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:33 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_dongle(t_simulation *sim, int dongle_id)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->lock);
	dongle->taken = 0;
	dongle->holder_id = -1;
	dongle->available_at = current_ms(sim) + sim->params.cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}
