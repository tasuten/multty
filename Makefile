CC = gcc
CFLAGS = -I.

LDFLAGS= -lutil

OBJECTS = multty.o session.o tab.o tty.o

MAIN = multty

all: $(MAIN)

multty: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(MAIN)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o $(MAIN)

.PHONY: all clean
