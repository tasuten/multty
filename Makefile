CC = gcc
CFLAGS = -I.

OBJECTS = multty.o session.o

MAIN = multty

all: $(MAIN)

multty: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(MAIN)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o $(MAIN)

.PHONY: all clean
