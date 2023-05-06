CC = gcc -I ./include -c
LD_FLAGS = -lxcb -lxcb-xtest


SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)


%.o: %.c
	$(CC) $< -o $@

all: $(OBJECTS)
	gcc $^ -o main $(LD_FLAGS)

clean:
	rm *.o main -rf
