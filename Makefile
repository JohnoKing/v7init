CC=gcc
CFLAGS=-O2 -o

all: getty init

getty:
	$(CC) $(CFLAGS) getty getty.c

init:
	$(CC) $(CFLAGS) init init.c

install: all
	install -Dm755 init getty $(DESTDIR)/sbin
	install -Dm755 rc $(DESTDIR)/etc

clean:
	rm -f getty init

clobber: clean
