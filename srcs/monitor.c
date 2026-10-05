/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:34 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:36 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnouts(t_simulation *sim, long now)
{
	int		i;
	long	deadline;

	i = 0;
	while (i < sim->params.coders)
	{
		deadline = get_last_compile_start(&sim->coders[i]);
		deadline += sim->params.burnout;
		if (now >= deadline)
		{
			set_stop_flag(sim);
			log_event(sim, sim->coders[i].id, MSG_BURNED_OUT);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_completion(t_simulation *sim)
{
	int		i;
	long	required;

	required = sim->params.compiles_required;
	i = 0;
	while (i < sim->params.coders)
	{
		if (get_compiles_done(&sim->coders[i]) < required)
			return (0);
		i++;
	}
	return (1);
}

void	run_monitor(t_simulation *sim)
{
	long	now;

	while (!simulation_stopped(sim))
	{
		now = current_ms(sim);
		if (check_burnouts(sim, now))
			return ;
		if (check_completion(sim))
		{
			set_stop_flag(sim);
			return ;
		}
		usleep(MONITOR_POLL_US);
	}
}

void	*monitor_routine(void *arg)
{
	run_monitor((t_simulation *)arg);
	return (NULL);
}
