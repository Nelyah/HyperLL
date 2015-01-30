CC=gcc
CFLAGS=-c -Wall -O3
LDFLAGS= -lm
SOURCES=murmur3.c hypLL64.c  sort.c
OBJECTS=$(SOURCES:.c=.o)
BINARY=hyperLL

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.c.o: 
	$(CC) $< -o $@ $(CFLAGS)

clean :
	rm -f *.o 
	rm -f $(BINARY)
