/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:11:23 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:11:26 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout \\\n");
	fprintf(stderr, "  time_to_compile time_to_debug time_to_refactor \\\n");
	fprintf(stderr, "  number_of_compiles_required dongle_cooldown \\\n");
	fprintf(stderr, "  scheduler(fifo|edf)\n");
	fprintf(stderr, "All numeric arguments must be non-negative integers.\n");
	fprintf(stderr, "Limits: 1-10000 coders, times <= 2147483647 ms.\n");
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	t_params		params;

	if (!parse_args(argc, argv, &params))
	{
		print_usage();
		return (1);
	}
	if (!init_simulation(&sim, &params))
	{
		cleanup_simulation(&sim);
		fprintf(stderr, "Error: codexion: initialization failed\n");
		return (1);
	}
	if (!start_threads(&sim))
	{
		cleanup_simulation(&sim);
		fprintf(stderr, "Error: codexion: thread creation failed\n");
		return (1);
	}
	join_threads(&sim);
	cleanup_simulation(&sim);
	return (0);
}
