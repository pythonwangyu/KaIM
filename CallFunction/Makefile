SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%,$(SRC))

CC = gcc
CFLAGS = -Wall -g

all:$(OBJ)


$(OBJ):%:%.c
	$(CC) $< -o $@ $(CFLAGS)


.PYONY:clean all

clean:
	rm -rf $(OBJ)

