CC=gcc
CFLAGS=-c -Wall -O3 
LDFLAGS=-lm
SOURCES=murmur3.c varint.c deltaVarIntEncoder.c deltaVarIntDecoder.c hypLL64.c sort.c bench3_64.c
OBJECTS=$(SOURCES:.c=.o)
BINARY=hyperLL

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o 
	rm -f $(BINARY)
