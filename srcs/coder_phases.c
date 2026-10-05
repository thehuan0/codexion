/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_phases.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:09:59 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:01 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sleep_interruptible(t_simulation *sim, long duration_ms)
{
	long	end;
	long	remaining;

	end = current_ms(sim) + duration_ms;
	while (1)
	{
		if (simulation_stopped(sim))
			return (-1);
		remaining = end - current_ms(sim);
		if (remaining <= 0)
			return (0);
		if (remaining > POLL_INTERVAL_MS)
			remaining = POLL_INTERVAL_MS;
		usleep(remaining * 1000);
	}
}

int	run_phase(t_simulation *sim, int coder_id, const char *msg, long dur)
{
	log_event(sim, coder_id, msg);
	return (sleep_interruptible(sim, dur));
}

int	run_compile_phase(t_coder *coder, int first, int second)
{
	t_simulation	*sim;
	int				ret;

	sim = coder->sim;
	set_last_compile_start(coder, current_ms(sim));
	log_event(sim, coder->id, MSG_COMPILING);
	ret = sleep_interruptible(sim, sim->params.compile_time);
	if (ret == 0)
		log_event(sim, coder->id, MSG_DEBUGGING);
	release_dongle(sim, first);
	release_dongle(sim, second);
	if (ret == -1)
		return (-1);
	increment_compiles_done(coder);
	return (0);
}
