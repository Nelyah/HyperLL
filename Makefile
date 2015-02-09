CC=gcc
CFLAGS=-c -Wall -O3 -g
LDFLAGS=-lm
SOURCES=murmur3.c varint.c deltaVarIntEncoder.c deltaVarIntDecoder.c hypLL64.c sort.c myMap.c bitStruct.c fromInput.c
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
