CC=gcc
CFLAGS=-O2 -o

all: getty init

getty:
	$(CC) $(CFLAGS) getty getty.c

init:
	$(CC) $(CFLAGS) init init.c

install: all
	cp init getty /sbin
	cp rc /etc
	chmod +x /etc/rc

clean:
	rm -f getty init

clobber: clean
