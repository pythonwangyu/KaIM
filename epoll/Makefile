src = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src))
target = $(patsubst %.c,%,$(src))


flag = -lpthread -g -Wall

all:$(target) $(obj)


%.o:%.c
	gcc -c $(<)
%:%.o
	gcc $(<) -o $(@) $(flag)

.PYONY:clean

clean:
	-rm -rf *.o $(target)
