/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:16 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:18 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	get_ordered_dongles(t_coder *coder, int *first, int *second)
{
	int	c;
	int	left;
	int	right;
	int	n;

	n = coder->sim->params.coders;
	c = coder->id - 1;
	right = c;
	left = (c - 1 + n) % n;
	if (left < right)
	{
		*first = left;
		*second = right;
		return ;
	}
	*first = right;
	*second = left;
}

int	try_compile(t_coder *coder)
{
	t_simulation	*sim;
	int				first;
	int				second;
	long			deadline;

	sim = coder->sim;
	get_ordered_dongles(coder, &first, &second);
	deadline = read_deadline(coder);
	if (acquire_dongle(sim, first, coder->id, deadline) == -1)
		return (-1);
	log_event(sim, coder->id, MSG_TAKEN);
	if (acquire_dongle(sim, second, coder->id, deadline) == -1)
	{
		release_dongle(sim, first);
		return (-1);
	}
	log_event(sim, coder->id, MSG_TAKEN);
	return (run_compile_phase(coder, first, second));
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!simulation_stopped(coder->sim))
	{
		if (try_compile(coder) == -1)
			break ;
		if (sleep_interruptible(coder->sim, coder->sim->params.debug_time)
			== -1)
			break ;
		if (run_phase(coder->sim, coder->id, MSG_REFACTORING,
				coder->sim->params.refactor_time) == -1)
			break ;
	}
	return (NULL);
}
