/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_scheduler.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:47 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:49 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_scheduler_arg(char **argv, t_params *params)
{
	if (strcmp(argv[8], "fifo") == 0)
	{
		params->scheduler = SCHED_MODE_FIFO;
		return (1);
	}
	if (strcmp(argv[8], "edf") == 0)
	{
		params->scheduler = SCHED_MODE_EDF;
		return (1);
	}
	return (0);
}
