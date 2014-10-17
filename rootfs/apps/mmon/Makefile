CC=armv5-linux-gcc
CFLAGS=-c -Wall 
LDFLAGS=
SOURCES=main.c xcom.c modem.c mmon.c xshared.c xconfig.c xutil.c xppp.c debug.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mmon

all: $(SOURCES) $(EXECUTABLE)
	cp $(EXECUTABLE) ../target/bin
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) *.o
