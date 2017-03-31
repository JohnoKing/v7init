CC=gcc
CFLAGS=-O2 -ffast-math -o

all: getty v7init

getty:
	$(CC) $(CFLAGS) getty getty.c

v7init:
	$(CC) $(CFLAGS) v7init v7init.c

install: all
	mkdir -p $(DESTDIR)/sbin $(DESTDIR)/etc/rc.d
	install -Dm755 init getty $(DESTDIR)/sbin
	install -Dm755 rc $(DESTDIR)/etc
	install -Dm755 rc.status services/dbus.sh $(DESTDIR)/etc/rc.d
	install -Dm644 ttys $(DESTDIR)/etc

clean:
	rm -f getty v7init

clobber: clean

mrproper: clean
