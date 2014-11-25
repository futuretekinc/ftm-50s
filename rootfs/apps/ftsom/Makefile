#CC=armv5-linux-gcc
CC=gcc
CFLAGS=-c -Wall -DDEBUG -I/usr/local/include 
LDFLAGS= -L/usr/local/lib -lconfig 
SOURCES=main.c xshared.c debug.c ftsom.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ftsom

all: $(SOURCES) $(EXECUTABLE)
	cp $(EXECUTABLE) ../target/bin
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -static -L/usr/local/lib/ -lconfig /usr/local/lib/libconfig.a

main.o: main.c ftsom.h
	$(CC) $(CFLAGS) $< -o $@

ftsom.o: ftsom.c ftsom.h
	$(CC) $(CFLAGS) $< -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) *.o
