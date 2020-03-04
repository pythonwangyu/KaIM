src = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src))
exe = $(patsubst %.c,%,$(src))




all:

clean:
	-rm *.o
