CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g

program: wish.o command_processing.o
	$(CC) $^ -o $@

wish.o: wish.c command_processing.h
	$(CC) $(CFLAGS) -c $<

command_processing.o: command_processing.c command_processing.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o program

run: program
	./program