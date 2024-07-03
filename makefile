CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g

wish: wish.o command_processing.o utils.o
	$(CC) $^ -o $@

wish.o: wish.c command_processing.h
	$(CC) $(CFLAGS) -c $<

command_processing.o: command_processing.c command_processing.h
	$(CC) $(CFLAGS) -c $<

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o wish

run: wish
	./wish $(filter-out $@,$(MAKECMDGOALS))

%:
	@: