CC = gcc
CFLAGS = -I.

OBJECTS = multty.o session.o

MAIN = multty

multty: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(MAIN)

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	-rm -f *.o $(MAIN)
