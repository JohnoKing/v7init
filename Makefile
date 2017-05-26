CC = cc
CFLAGS = -O2 -ffast-math -ftree-vectorize -fomit-frame-pointer -frename-registers -fweb -flto -fuse-linker-plugin
CPPFLAGS = -DAGETTY
LDFLAGS = -Wl,--sort-common,--as-needed,--hash-style=gnu,-O1 $(CFLAGS)
STRIP = strip

all: v7init

v7init:
	$(CC) $(CFLAGS) $(CPPFLAGS) -o v7init v7init.c $(LDFLAGS)

strip:
	$(STRIP) -s -R .comment -R .note.gnu.build-id v7init

install:
	mkdir -p $(DESTDIR)/etc/v7init/en_srv $(DESTDIR)/sbin
	cp -r srv $(DESTDIR)/etc/v7init
	install -Dm755 v7init v7poweroff v7srv v7reboot $(DESTDIR)/sbin
	install -Dm755 rc $(DESTDIR)/etc/v7init
	install -Dm644 ttys $(DESTDIR)/etc/v7init

uninstall:
	rm -rf /sbin/v7init /sbin/v7poweroff /sbin/v7srv /sbin/v7reboot /etc/v7init

clean:
	rm -f v7init

clobber: clean
