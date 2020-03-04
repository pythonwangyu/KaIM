src = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src))
exe = $(patsubst %.c,%,$(src))




