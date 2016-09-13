#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define	TABSIZ	100
#define	ALL	p = &itab[0]; p < &itab[TABSIZ]; p++
#define	EVER	;;

char	shell[]	= "/bin/sh";
char	getty[]	= "/sbin/getty-v7";
char	minus[]	= "-";
char	runc[]	= "/etc/rc-v7";
char	ifile[]	= "/etc/ttys";
char	utmp[]	= "/etc/utmp";
char	wtmpf[]	= "/var/log/wtmp";
char	ctty[]	= "/dev/console";
char	dev[]	= "/dev/";

struct utmp wtmp;
struct
{
	char	line[8];
	char	comn;
	char	flag;
} line;
struct	tab
{
	char	line[8];
	char	comn;
	int	pid;
} itab[TABSIZ];

int	fi;
char	tty[20];
jmp_buf	sjbuf;

int reset()
{
	longjmp(sjbuf, 1);
}

void rmut(p)
register struct tab *p;
{
	register int i, f;

	f = open(utmp, 2);
	if(f >= 0) {
		while(read(f, (char *)&wtmp, sizeof(wtmp)) == sizeof(wtmp)) {
			for(i=0; i<8; i++)
				if(wtmp.ut_line[i] != p->line[i])
					goto contin;
			lseek(f, -(long)sizeof(wtmp), 1);
			for(i=0; i<8; i++)
				wtmp.ut_name[i] = 0;
			time(&wtmp.ut_time);
			write(f, (char *)&wtmp, sizeof(wtmp));
		contin:;
		}
		close(f);
	}
	f = open(wtmpf, 1);
	if (f >= 0) {
		for(i=0; i<8; i++) {
			wtmp.ut_name[i] = 0;
			wtmp.ut_line[i] = p->line[i];
		}
		time(&wtmp.ut_time);
		lseek(f, (long)0, 2);
		write(f, (char *)&wtmp, sizeof(wtmp));
		close(f);
	}
}

void term(p)
register struct tab *p;
{

	if(p->pid != 0) {
		rmut(p);
		kill(p->pid, SIGKILL);
	}
	p->pid = 0;
	p->line[0] = 0;
}

void shutdown()
{
	register int i;
	register struct tab *p;

	signal(SIGINT, SIG_IGN);
	for(ALL)
		term(p);
	signal(SIGALRM, reset);
	alarm(60);
	for(i=0; i<5; i++)
		kill(-1, SIGKILL);
	while(wait((int *)0) != -1)
		;
	alarm(0);
	signal(SIGALRM, SIG_DFL);
	for(i=0; i<10; i++)
		close(i);
}

void single()
{
	register int pid;

	pid = fork();
	if(pid == 0) {
/*
		alarm(300);
*/
		signal(SIGHUP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGALRM, SIG_DFL);
		open(ctty, 2);
		dup(0);
		dup(0);
		execl(shell, minus, (char *)0);
		exit(0);
	}
	while(wait((int *)0) != pid)
		;
}

void runcom()
{
	register int pid;

	pid = fork();
	if(pid == 0) {
		open("/", 0);
		dup(0);
		dup(0);
		execl(shell, shell, runc, (char *)0);
		exit(0);
	}
	while(wait((int *)0) != pid)
		;
}

void maktty(char *lin)
{
	register int i, j;

	for(i=0; dev[i]; i++)
		tty[i] = dev[i];
	for(j=0; lin[j]; j++) {
		tty[i] = lin[j];
		i++;
	}
	tty[i] = 0;
}

void dfork(p)
struct tab *p;
{
	register int pid;

	pid = fork();
	if(pid == 0) {
		signal(SIGHUP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		maktty(p->line);
		chown(tty, 0, 0);
		chmod(tty, 0622);
		open(tty, 2);
		dup(0);
		dup(0);
		tty[0] = p->comn;
		tty[1] = 0;
		execl(getty, minus, tty, (char *)0);
		exit(0);
	}
	p->pid = pid;
}

int get()
{
	char b;

	if(read(fi, &b, 1) != 1)
		return(-1);
	if(b == '\n')
		return(0);
	return(b);
}

int rline()
{
	register int c, i;

	c = get();
	if(c < 0)
		return(0);
	if(c == 0)
		goto bad;
	line.flag = c;
	c = get();
	if(c <= 0)
		goto bad;
	line.comn = c;
	for(i=0; i<8; i++)
		line.line[i] = 0;
	for(i=0; i<7; i++) {
		c = get();
		if(c <= 0)
			break;
		line.line[i] = c;
	}
	while(c > 0)
		c = get();
	maktty(line.line);
	if(access(tty, 06) < 0)
		goto bad;
	return(1);

bad:
	line.flag = '0';
	return(1);
}

void merge()
{
	register struct tab *p, *q;
	register int i;

	close(creat(utmp, 0644));
	signal(SIGINT, merge);
	fi = open(ifile, 0);
	if(fi < 0)
		return;
	q = &itab[0];
	while(rline()) {
		if(line.flag == '0')
			continue;
		for(ALL) {
			if(p->line[0] != 0)
			for(i=0; i<8; i++)
				if(p->line[i] != line.line[i])
					goto contin;
			if(p >= q) {
				i = p->pid;
				p->pid = q->pid;
				q->pid = i;
				for(i=0; i<8; i++)
					p->line[i] = q->line[i];
				p->comn = q->comn;
				for(i=0; i<8; i++)
					q->line[i] = line.line[i];
				q->comn = line.comn;
				q++;
			}
			break;
		contin:
			;
		}
	}
	close(fi);
	for(; q < &itab[TABSIZ]; q++)
		term(q);
	for(ALL)
		if(p->line[0] != 0 && p->pid == 0)
			dfork(p);
}

void multiple()
{
	register struct tab *p;
	register int pid;

	for(EVER) {
		pid = wait((int *)0);
		if(pid == -1)
			return;
		for(ALL)
			if(p->pid == pid || p->pid == -1) {
				rmut(p);
				dfork(p);
			}
	}
}

int main()
{
	int reset();

	setjmp(sjbuf);
	signal(SIGHUP, reset);
	for(EVER) {
		shutdown();
		single();
		runcom();
		merge();
		multiple();
	}
}
