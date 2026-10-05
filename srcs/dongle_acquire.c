/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:26 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:29:01 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	build_request_node(t_simulation *sim, int coder_id, long deadline,
		t_heap_node *node)
{
	long	seq;

	seq = next_sequence(sim);
	node->coder_id = coder_id;
	node->secondary = seq;
	if (sim->params.scheduler == SCHED_MODE_FIFO)
		node->primary = seq;
	else
		node->primary = deadline;
}

static int	dongle_ready_for(t_dongle *dongle, int coder_id, long now)
{
	t_heap_node	top;

	if (dongle->taken)
		return (0);
	if (now < dongle->available_at)
		return (0);
	if (!heap_peek_min(&dongle->waiters, &top))
		return (0);
	return (top.coder_id == coder_id);
}

static void	compute_wait_time(t_dongle *dongle, long now,
		struct timespec *ts, t_simulation *sim)
{
	long	target;
	long	abs_us;

	target = now + POLL_INTERVAL_MS;
	if (!dongle->taken && dongle->available_at > now
		&& dongle->available_at < target)
		target = dongle->available_at;
	abs_us = sim_start_epoch_us(sim) + target * 1000L;
	ts->tv_sec = abs_us / 1000000L;
	ts->tv_nsec = (abs_us % 1000000L) * 1000L;
}

static int	finalize_acquire(t_simulation *sim, t_dongle *dongle, int id)
{
	heap_remove_coder(&dongle->waiters, id);
	if (simulation_stopped(sim))
	{
		pthread_mutex_unlock(&dongle->lock);
		return (-1);
	}
	dongle->taken = 1;
	dongle->holder_id = id;
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

int	acquire_dongle(t_simulation *sim, int dongle_id, int coder_id,
		long deadline)
{
	t_dongle		*dongle;
	t_heap_node		node;
	struct timespec	ts;

	dongle = &sim->dongles[dongle_id];
	build_request_node(sim, coder_id, deadline, &node);
	pthread_mutex_lock(&dongle->lock);
	if (!heap_push(&dongle->waiters, node))
	{
		pthread_mutex_unlock(&dongle->lock);
		return (-1);
	}
	while (!dongle_ready_for(dongle, coder_id, current_ms(sim))
		&& !simulation_stopped(sim))
	{
		if (!dongle->taken && current_ms(sim) >= dongle->available_at)
			pthread_cond_broadcast(&dongle->cond);
		compute_wait_time(dongle, current_ms(sim), &ts, sim);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
	}
	return (finalize_acquire(sim, dongle, coder_id));
}
