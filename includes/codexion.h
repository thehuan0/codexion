/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::  */
/*   codexion.h                                        :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+    */
/*   By: jperez-s <jperez-s@student.42madrid.com> +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+          */
/*   Created: 2026/06/24 00:00:00 by jperez-s         #+#    #+#            */
/*   Updated: 2026/06/24 00:00:00 by jperez-s        ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>

# define POLL_INTERVAL_MS 5
# define MONITOR_POLL_US 1000
# define DONGLE_QUEUE_CAP 2
# define MAX_CODERS 10000
# define MAX_TIME_MS INT_MAX

# define MSG_TAKEN "has taken a dongle"
# define MSG_COMPILING "is compiling"
# define MSG_DEBUGGING "is debugging"
# define MSG_REFACTORING "is refactoring"
# define MSG_BURNED_OUT "burned out"

typedef enum e_scheduler
{
	SCHED_MODE_FIFO,
	SCHED_MODE_EDF
}	t_scheduler;

typedef struct s_heap_node
{
	long	primary;
	long	secondary;
	int		coder_id;
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node	*nodes;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	int				taken;
	int				holder_id;
	long			available_at;
	t_heap			waiters;
}	t_dongle;

typedef struct s_params
{
	int			coders;
	long		burnout;
	long		compile_time;
	long		debug_time;
	long		refactor_time;
	long		compiles_required;
	long		cooldown;
	t_scheduler	scheduler;
}	t_params;

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int				id;
	long			last_compile_start;
	long			compiles_done;
	pthread_mutex_t	state_lock;
	t_simulation	*sim;
}	t_coder;

struct s_simulation
{
	t_params		params;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_t		*threads;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	seq_lock;
	pthread_t		monitor;
	int				stop;
	int				started;
	int				monitor_started;
	int				sync_ready;
	int				dongles_ready;
	int				coders_ready;
	long			request_seq;
	struct timeval	start_time;
};

/* parsing_utils.c */
int		is_digit_char(char c);
int		is_valid_digit_string(const char *str);
int		string_to_long(const char *str, long *out);
int		parse_numeric_field(const char *str, long *out);

/* parsing_scheduler.c */
int		parse_scheduler_arg(char **argv, t_params *params);

/* parsing_args.c */
int		parse_args(int argc, char **argv, t_params *params);
int		parse_numeric_args_a(char **argv, t_params *params);
int		parse_numeric_args_b(char **argv, t_params *params);
int		validate_ranges(t_params *params);

/* main.c */
void	print_usage(void);

/* init_simulation.c */
int		init_simulation(t_simulation *sim, t_params *params);
int		init_sync_primitives(t_simulation *sim);

/* init_dongles.c */
int		init_dongles(t_simulation *sim);
int		init_one_dongle(t_dongle *dongle, int capacity);

/* init_coders.c */
int		init_coders(t_simulation *sim);
int		init_one_coder(t_coder *coder, t_simulation *sim, int id);

/* cleanup_simulation.c */
void	cleanup_simulation(t_simulation *sim);
void	destroy_dongles(t_simulation *sim);
void	destroy_coders(t_simulation *sim);
void	destroy_sync(t_simulation *sim);

/* threads.c */
int		start_coders(t_simulation *sim);
int		start_monitor(t_simulation *sim);
int		start_threads(t_simulation *sim);
void	join_threads(t_simulation *sim);

/* heap_core.c */
int		heap_init(t_heap *heap, int capacity);
void	heap_destroy(t_heap *heap);
int		heap_is_empty(t_heap *heap);
int		heap_compare(t_heap_node *a, t_heap_node *b);
void	heap_swap(t_heap_node *a, t_heap_node *b);

/* heap_push_pop.c */
int		heap_push(t_heap *heap, t_heap_node node);
int		heap_pop_min(t_heap *heap, t_heap_node *out);
int		heap_peek_min(t_heap *heap, t_heap_node *out);

/* heap_sift.c */
void	heap_sift_up(t_heap *heap, int index);
void	heap_sift_down(t_heap *heap, int index);
int		heap_find_coder_index(t_heap *heap, int coder_id);
int		heap_remove_coder(t_heap *heap, int coder_id);

/* dongle_acquire.c */
int		acquire_dongle(t_simulation *sim, int dongle_id, int coder_id,
			long deadline);

/* dongle_release.c */
void	release_dongle(t_simulation *sim, int dongle_id);

/* sim_state.c */
long	current_ms(t_simulation *sim);
long	sim_start_epoch_us(t_simulation *sim);
long	next_sequence(t_simulation *sim);
int		simulation_stopped(t_simulation *sim);
void	set_stop_flag(t_simulation *sim);

/* log.c */
void	log_event(t_simulation *sim, int coder_id, const char *msg);

/* coder_state.c */
void	set_last_compile_start(t_coder *coder, long value);
void	increment_compiles_done(t_coder *coder);
long	read_deadline(t_coder *coder);
long	get_compiles_done(t_coder *coder);
long	get_last_compile_start(t_coder *coder);

/* coder_phases.c */
int		run_compile_phase(t_coder *coder, int first, int second);
int		run_phase(t_simulation *sim, int coder_id, const char *msg, long dur);
int		sleep_interruptible(t_simulation *sim, long duration_ms);

/* coder.c */
void	*coder_routine(void *arg);
int		try_compile(t_coder *coder);
void	get_ordered_dongles(t_coder *coder, int *first, int *second);

/* monitor.c */
void	run_monitor(t_simulation *sim);
void	*monitor_routine(void *arg);
int		check_burnouts(t_simulation *sim, long now);
int		check_completion(t_simulation *sim);

#endif
