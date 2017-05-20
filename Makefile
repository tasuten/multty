CC = gcc
CFLAGS = -Wall -Wextra -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith -Winit-self -Wpadded -I.

LDFLAGS= -lutil -lpthread

TARGET = multty

SOURCES = $(wildcard *.c)
OBJDIR = ./obj
OBJECTS   = $(addprefix $(OBJDIR)/,  $(filter-out $(TARGET).o, $(notdir $(SOURCES:.c=.o))))


# $^ : string that joined depending file names with a space
# $@ : target name
$(TARGET): $(OBJECTS) $(TARGET).c
	$(CC) $(LDFLAGS) $^ -o $@

# $< : depending file name
$(OBJDIR)/%.o: %.c
	-mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
