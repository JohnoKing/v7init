CC = cc
CFLAGS = -O2 -ffast-math -ftree-vectorize -fomit-frame-pointer -frename-registers -fweb -flto -fuse-linker-plugin
LDFLAGS = -Wl,--sort-common,--as-needed,--hash-style=gnu,-O1 $(CFLAGS)
STRIP= strip

all: v7getty v7init

getty:
	$(CC) $(CFLAGS) -o v7getty v7getty.c $(LDFLAGS)

v7init:
	$(CC) $(CFLAGS) -o v7init v7init.c $(LDFLAGS)

strip:
	$(STRIP) -s -R .comment -R .note.gnu.build-id v7getty v7init

install: all
	mkdir -p $(DESTDIR)/sbin $(DESTDIR)/etc/v7init/en $(DESTDIR)/etc/v7init/ne
	install -Dm755 v7getty v7init v7poweroff v7srv v7reboot $(DESTDIR)/sbin
	install -Dm755 rc $(DESTDIR)/etc/v7init
	install -Dm644 ttys $(DESTDIR)/etc/v7init
	install -Dm644 services/*.sh $(DESTDIR)/etc/v7init/ne

uninstall:
	rm -rf /sbin/v7getty /sbin/v7init /sbin/v7srv /etc/v7init

clean:
	rm -f v7getty v7init

clobber: clean

mrproper: clean
