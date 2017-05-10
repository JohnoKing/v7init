CC=cc
CFLAGS=-O2 -ffast-math -fomit-frame-pointer -frename-registers -fweb -flto -fuse-linker-plugin

all: v7getty v7init

getty:
	$(CC) $(CFLAGS) -o v7getty v7getty.c

v7init:
	$(CC) $(CFLAGS) -o v7init v7init.c

install: all
	mkdir -p $(DESTDIR)/sbin $(DESTDIR)/etc/v7init/rc.d
	install -Dm755 v7init v7getty $(DESTDIR)/sbin
	install -Dm755 rc ttys $(DESTDIR)/etc/v7init
	install -Dm755 rc.status services/dbus.sh $(DESTDIR)/etc/v7init/rc.d

clean:
	rm -f getty v7init

clobber: clean

mrproper: clean
