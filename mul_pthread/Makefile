src = $(wildcard *.c)
mid = $(patsubst %.c,%.o,$(src))
fin = $(patsubst %.c,%,$(src))

ko = -lpthread -g -Wall

all:$(mid) $(fin)


%.o:%.c
	gcc -c $<   

%:%.c
	gcc $^ -o $@  $(ko)


.PYONY:clean deletecfile

clean:
	-rm -rf *.o $(fin)

deletecfile:
	-rm -rf *.c




