/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:08 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:12 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	set_last_compile_start(t_coder *coder, long value)
{
	pthread_mutex_lock(&coder->state_lock);
	coder->last_compile_start = value;
	pthread_mutex_unlock(&coder->state_lock);
}

void	increment_compiles_done(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_lock);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->state_lock);
}

long	read_deadline(t_coder *coder)
{
	long	value;

	pthread_mutex_lock(&coder->state_lock);
	value = coder->last_compile_start + coder->sim->params.burnout;
	pthread_mutex_unlock(&coder->state_lock);
	return (value);
}

long	get_compiles_done(t_coder *coder)
{
	long	value;

	pthread_mutex_lock(&coder->state_lock);
	value = coder->compiles_done;
	pthread_mutex_unlock(&coder->state_lock);
	return (value);
}

long	get_last_compile_start(t_coder *coder)
{
	long	value;

	pthread_mutex_lock(&coder->state_lock);
	value = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_lock);
	return (value);
}
