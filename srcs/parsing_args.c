/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_args.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:40 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:23:04 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_numeric_args_a(char **argv, t_params *params)
{
	long	coders;
	long	burnout;
	long	compile;
	long	debug;

	if (!parse_numeric_field(argv[1], &coders))
		return (0);
	if (!parse_numeric_field(argv[2], &burnout))
		return (0);
	if (!parse_numeric_field(argv[3], &compile))
		return (0);
	if (!parse_numeric_field(argv[4], &debug))
		return (0);
	if (coders > MAX_CODERS)
		return (0);
	params->coders = (int)coders;
	params->burnout = burnout;
	params->compile_time = compile;
	params->debug_time = debug;
	return (1);
}

int	parse_numeric_args_b(char **argv, t_params *params)
{
	long	refactor;
	long	required;
	long	cooldown;

	if (!parse_numeric_field(argv[5], &refactor))
		return (0);
	if (!parse_numeric_field(argv[6], &required))
		return (0);
	if (!parse_numeric_field(argv[7], &cooldown))
		return (0);
	params->refactor_time = refactor;
	params->compiles_required = required;
	params->cooldown = cooldown;
	return (1);
}

static int	time_in_range(long value)
{
	return (value >= 0 && value <= MAX_TIME_MS);
}

int	validate_ranges(t_params *params)
{
	if (params->coders < 1 || params->coders > MAX_CODERS)
		return (0);
	if (params->burnout < 1)
		return (0);
	if (!time_in_range(params->burnout)
		|| !time_in_range(params->compile_time)
		|| !time_in_range(params->debug_time))
		return (0);
	if (!time_in_range(params->refactor_time)
		|| !time_in_range(params->cooldown)
		|| !time_in_range(params->compiles_required))
		return (0);
	return (1);
}

int	parse_args(int argc, char **argv, t_params *params)
{
	if (argc != 9)
		return (0);
	if (!parse_numeric_args_a(argv, params))
		return (0);
	if (!parse_numeric_args_b(argv, params))
		return (0);
	if (!parse_scheduler_arg(argv, params))
		return (0);
	if (!validate_ranges(params))
		return (0);
	return (1);
}
