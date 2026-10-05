/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:58 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:58 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_one_coder(t_coder *coder, t_simulation *sim, int id)
{
	coder->id = id;
	coder->sim = sim;
	coder->last_compile_start = 0;
	coder->compiles_done = 0;
	return (pthread_mutex_init(&coder->state_lock, NULL) == 0);
}

int	init_coders(t_simulation *sim)
{
	int	i;
	int	n;

	n = sim->params.coders;
	sim->coders = malloc(sizeof(t_coder) * n);
	if (sim->coders == NULL)
		return (0);
	sim->threads = malloc(sizeof(pthread_t) * n);
	if (sim->threads == NULL)
		return (0);
	i = 0;
	while (i < n)
	{
		if (!init_one_coder(&sim->coders[i], sim, i + 1))
			return (0);
		sim->coders_ready++;
		i++;
	}
	return (1);
}
