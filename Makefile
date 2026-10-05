NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC_DIR = srcs
OBJ_DIR = objs
INC_DIR = includes

SRCS = main.c \
	parsing_utils.c \
	parsing_scheduler.c \
	parsing_args.c \
	init_simulation.c \
	init_dongles.c \
	init_coders.c \
	cleanup_simulation.c \
	threads.c \
	heap_core.c \
	heap_push_pop.c \
	heap_sift.c \
	sim_state.c \
	log.c \
	dongle_acquire.c \
	dongle_release.c \
	coder_state.c \
	coder_phases.c \
	coder.c \
	monitor.c

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): $(INC_DIR)/codexion.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
