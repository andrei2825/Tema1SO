CC = gcc
CFLAGS = -Wall -g

build: so-cpp

so-cpp: so-cpp.o
	$(CC) $^ -o $@

so-cpp.o: so-cpp.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *~ so-cpp