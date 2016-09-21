CC=gcc
CFLAGS=-c -O2
LDFLAGS=-o

all: getty.o init.o

getty.o:
	$(CC) $(CFLAGS) getty.c
	$(CC) $(LDFLAGS) getty getty.o

init.o:
	$(CC) $(CFLAGS) init.c
	$(CC) $(LDFLAGS) init init.o

install: all
	cp init getty /sbin
	cp rc /etc

clean:
	rm *.o

clobber: clean
	rm getty init
