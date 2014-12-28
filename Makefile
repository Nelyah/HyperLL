CC=gcc
CFLAGS=-c -Wall -O3
LDFLAGS=
SOURCES=murmur3.c hypLL.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=hyperLL

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o 
	rm -f $(EXECUTABLE)
