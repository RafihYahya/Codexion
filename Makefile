# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -pthread -fsanitize=address -g3 -DDEBUG_MODE -DSKIP_PARSE -DTESTING

# Output name
NAME = codexion

# Source directory
SRC_DIR = coders

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/monitor.c \
       $(SRC_DIR)/mutex.c \
       $(SRC_DIR)/output_logger.c \
       $(SRC_DIR)/parser.c \
       $(SRC_DIR)/scheduler.c \
       $(SRC_DIR)/scheduler_edf.c \
       $(SRC_DIR)/scheduler_fifo.c \
       $(SRC_DIR)/thread.c \
       $(SRC_DIR)/time.c

# Object files (place .o files next to Makefile)
OBJS = $(SRCS:$(SRC_DIR)/%.c=%.o)

# Default rule
all: $(NAME)

# Link
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# Compile
%.o: $(SRC_DIR)/%.c $(SRC_DIR)/main.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -f *.o

# Clean everything
fclean: clean
	rm -f $(NAME)

# Rebuild
re: fclean all

.PHONY: all clean fclean re