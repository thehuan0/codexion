/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:17 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:19 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_event(t_simulation *sim, int coder_id, const char *msg)
{
	long	now;

	pthread_mutex_lock(&sim->log_lock);
	if (!simulation_stopped(sim) || strcmp(msg, MSG_BURNED_OUT) == 0)
	{
		now = current_ms(sim);
		printf("%ld %d %s\n", now, coder_id, msg);
	}
	pthread_mutex_unlock(&sim->log_lock);
}
