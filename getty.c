/*
 * getty -- adapt to terminal speed on dialup, and call login
 */

#include <signal.h>
#include <stdlib.h>
#include <stropts.h>
#include <unistd.h>

#include "libstty/sgtty.h"

#define ERASE	'#'
#define KILL	'@'

struct sgttyb tmode;
struct tchars tchars = { '\177', '\034', '\021', '\023', '\004', '\377' };

struct	tab {
	char	tname;		/* this table name */
	char	nname;		/* successor table name */
	int	iflags;		/* initial flags */
	int	fflags;		/* final flags */
	int	ispeed;		/* input speed */
	int	ospeed;		/* output speed */
};

#define	EOT	04		/* EOT char */

char	name[16];
int	crmod;
int	upper;
int	lower;

char partab[] = {
	0001,0201,0201,0001,0201,0001,0001,0201,
	0202,0004,0003,0205,0005,0206,0201,0001,
	0201,0001,0001,0201,0001,0201,0201,0001,
	0001,0201,0201,0001,0201,0001,0001,0201,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0201
};

void putchr(cc)
{
	char c;
	c = cc;
	c |= partab[c&0177] & 0200;
	write(1, &c, 1);
}

void puts(char *as)
{
	register char *s;

	s = as;
	while (*s)
		putchr(*s++);
}

int getname()
{
	register char *np;
	register int c;
	char cs;

	crmod = 0;
	upper = 0;
	lower = 0;
	np = name;
	for (;;) {
		if (read(0, &cs, 1) <= 0)
			exit(0);
		if ((c = cs&0177) == 0)
			return(0);
		if (c==EOT)
			exit(1);
		if (c=='\r' || c=='\n' || np >= &name[16])
			break;
		putchr(cs);
		if (c>='a' && c <='z')
			lower++;
		else if (c>='A' && c<='Z') {
			upper++;
			c += 'a'-'A';
		} else if (c==ERASE) {
			if (np > name)
				np--;
			continue;
		} else if (c==KILL) {
			putchr('\r');
			putchr('\n');
			np = name;
			continue;
		} else if(c == ' ')
			c = '_';
		*np++ = c;
	}
	*np = 0;
	if (c == '\r')
		crmod++;
	return(1);
}

int main(int argc, char **argv)
{
	int tname;

	tname = '0';
	if (argc > 1)
		tname = argv[1][0];
	switch (tname) {

	case '3':		/* adapt to connect speed (212) */
		ioctl(0, TIOCGETP, &tmode);
		if (tmode.sg_ispeed==B300)
			tname = '0';
		else
			tname = '3';
		break;
	}
	for (;;) {
		ioctl(0, TIOCSETP, &tmode);
		ioctl(0, TIOCSETC, &tchars);
		puts("login: ");
		if(getname()) {
			tmode.sg_erase = ERASE;
			tmode.sg_kill = KILL;
			if(crmod)
				tmode.sg_flags |= CRMOD;
			if(upper)
				tmode.sg_flags |= LCASE;
			if(lower)
				tmode.sg_flags &= ~LCASE;
			stty(0, &tmode);
			putchr('\n');
			execl("/bin/login", "login", name, 0);
			exit(1);
		}
	}
}
