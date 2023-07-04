CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LIBFLAGS = -lssh
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = prog

all: $(OBJ)
	$(CC) $(OBJ) $(LIBFLAGS) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o