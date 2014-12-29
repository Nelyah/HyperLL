CC=gcc
CFLAGS=-c -Wall -O3
LDFLAGS=
SOURCES=murmur3.c hypLL_noThread.c
OBJECTS=$(SOURCES:.c=.o)
BINARY=hyperLL

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o 
	rm -f $(BINARY)
