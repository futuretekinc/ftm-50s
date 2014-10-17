CC=armv5-linux-gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c xftmbcc.c xserial.c tx200k.c xshared.c xutil.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ftmbcc

all: $(SOURCES) $(EXECUTABLE)
	cp $(EXECUTABLE) ../target/bin
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) *.o
