CC=armv5-linux-gcc
AR=armv5-linux-ar
CFLAGS=-c -fPIC -Wall -DDEBUG -I../devtools/include
LDFLAGS=-L../devtools/lib -lconfig -lftsom
#CC=gcc
#CFLAGS=-c -Wall -DDEBUG -I/usr/local/include 
#LDFLAGS= -L/usr/local/lib -lconfig 

TARGET=ftsom
TARGET_SRCS=main.c 
TARGET_OBJS=$(TARGET_SRCS:.c=.o)

LIB_TARGET=libftsom.so.1.0.1
LIB_INCS= xshared.h debug.h ftsom.h
LIB_SRCS= xshared.c debug.c ftsom.c
LIB_OBJS=$(LIB_SRCS:.c=.o)


all: $(TARGET) $(LIB_TARGET)

install: $(TARGET)
	cp $(TARGET) ../target/bin

lib_install: $(LIB_TARGET)
	for h in $(LIB_INCS); do cp $$h ../devtools/include ; done
		
	cp $(LIB_TARGET) ../devtools/lib
	if [ -e ../devtools/lib/lib$(TARGET).so.1 ];\
	then\
		rm ../devtools/lib/lib$(TARGET).so.1;\
	fi

	ln -s $(LIB_TARGET) ../devtools/lib/lib$(TARGET).so.1	

	if [ -e ../devtools/lib/lib$(TARGET).so ];\
	then\
		rm ../devtools/lib/lib$(TARGET).so;\
	fi

	ln -s lib$(TARGET).so.1 ../devtools/lib/lib$(TARGET).so	

$(TARGET): $(TARGET_OBJS)
	$(CC) -o $@ $(TARGET_OBJS) $(LDFLAGS)

$(LIB_TARGET): $(LIB_OBJS)
	$(AR) rcs lib$(TARGET).a $(LIB_OBJS)
	$(CC) -shared -Wl,-soname,lib$(TARGET).so.1 -o lib$(TARGET).so.1.0.1 $(LIB_OBJS)

main.o: main.c ftsom.h
	$(CC) $(CFLAGS) $< -o $@

ftsom.o: ftsom.c ftsom.h
	$(CC) $(CFLAGS) $< -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(TARGET) *.o
