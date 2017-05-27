/*
 * v7init -- init program from UNIX V7
 */

#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>

#define TABSIZ 100
#define ALL            \
    p = &itab[0];      \
    p < &itab[TABSIZ]; \
    p++
#define ever ;;

#ifdef AGETTY
static char getty[] = "/sbin/agetty";
#else
static char getty[] = "/sbin/getty";
#endif

static char shell[] = "/bin/sh";
static char minus[] = "-";
static char runc[] = "/etc/v7init/rc";
static char ifile[] = "/etc/v7init/ttys";
static char utmp[] = "/etc/utmp";
static char wtmpf[] = "/var/log/wtmp";
static char ctty[] = "/dev/console";
static char dev[] = "/dev/";

struct utmp static wtmp;

struct {
    char line[8];
    char comn;
    char flag;
} static line;

struct tab {
    char line[8];
    char comn;
    int pid;
} static itab[TABSIZ];

static int fi;
static char tty[20];
static jmp_buf sjbuf;

static int reset(void)
{
    longjmp(sjbuf, 1);
}

static void rmut(struct tab *p)
{
    int i, f = open(utmp, 2);

    if (f >= 0) {
        while (read(f, (char*)&wtmp, sizeof(wtmp)) == sizeof(wtmp)) {
            for (i = 0; i < 8; i++)
                if (wtmp.ut_line[i] != p->line[i])
                    goto contin;
            lseek(f, -(long)sizeof(wtmp), 1);
            for (i = 0; i < 8; i++)
                wtmp.ut_name[i] = 0;
            time(&wtmp.ut_time);
            write(f, (char*)&wtmp, sizeof(wtmp));
        contin:;
        }
        close(f);
    }

    f = open(wtmpf, 1);
    if (f >= 0) {
        for (i = 0; i < 8; i++) {
            wtmp.ut_name[i] = 0;
            wtmp.ut_line[i] = p->line[i];
        }
        time(&wtmp.ut_time);
        lseek(f, (long)0, 2);
        write(f, (char*)&wtmp, sizeof(wtmp));
        close(f);
    }
}

static void term(struct tab *p)
{
    if (p->pid != 0) {
        rmut(p);
        kill(p->pid, SIGKILL);
    }
    p->pid = 0;
    p->line[0] = 0;
}

static void shutdown(void)
{
    int i;
    struct tab *p;

    signal(SIGINT, SIG_IGN);
    for (ALL)
        term(p);
    signal(SIGALRM, (void*)reset);
    alarm(60);
    for (i = 0; i < 5; i++)
        kill(-1, SIGKILL);
    while (wait((int*)0) != -1)
        ;
    alarm(0);
    signal(SIGALRM, SIG_DFL);
    for (i = 0; i < 10; i++)
        close(i);
}

static void runcom(void)
{
    int pid = fork();

    if (pid == 0) {
        open("/", 0);
        dup(0);
        dup(0);
        execl(shell, shell, runc, (char*)0);
        exit(0);
    }
    while (wait((int*)0) != pid)
        ;
}

static void maktty(char *lin)
{
    int i, j;

    for (i = 0; dev[i]; i++)
        tty[i] = dev[i];
    for (j = 0; lin[j]; j++) {
        tty[i] = lin[j];
        i++;
    }
    tty[i] = 0;
}

static void dfork(struct tab *p)
{
    int pid = fork();

    if (pid == 0) {
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
        execl(getty, minus, tty, (char*)0);
        exit(0);
    }
    p->pid = pid;
}

static int get(void)
{
    char b;

    if (read(fi, &b, 1) != 1)
        return -1;
    if (b == '\n')
        return 0;
    return b;
}

static int rline(void)
{
    int i, c = get();

    if (c < 0)
        return 0;
    if (c == 0)
        goto bad;
    line.flag = c;
    c = get();
    if (c <= 0)
        goto bad;
    line.comn = c;
    for (i = 0; i < 8; i++)
        line.line[i] = 0;
    for (i = 0; i < 7; i++) {
        c = get();
        if (c <= 0)
            break;
        line.line[i] = c;
    }
    while (c > 0)
        c = get();
    maktty(line.line);
    if (access(tty, 06) < 0)
        goto bad;
    return 1;

bad:
    line.flag = '0';
    return 1;
}

static void merge()
{
    struct tab *p, *q;
    int i;

    close(creat(utmp, 0644));
    signal(SIGINT, merge);
    fi = open(ifile, 0);
    if (fi < 0)
        return;
    q = &itab[0];
    while (rline()) {
        if (line.flag == '0')
            continue;
        for (ALL) {
            if (p->line[0] != 0)
                for (i = 0; i < 8; i++)
                    if (p->line[i] != line.line[i])
                        goto contin;
            if (p >= q) {
                i = p->pid;
                p->pid = q->pid;
                q->pid = i;
                for (i = 0; i < 8; i++)
                    p->line[i] = q->line[i];
                p->comn = q->comn;
                for (i = 0; i < 8; i++)
                    q->line[i] = line.line[i];
                q->comn = line.comn;
                q++;
            }
            break;
        contin:;
        }
    }
    close(fi);
    for (; q < &itab[TABSIZ]; q++)
        term(q);
    for (ALL)
        if (p->line[0] != 0 && p->pid == 0)
            dfork(p);
}

static void multiple(void)
{
    struct tab *p;
    int pid;

    for(ever) {
        pid = wait((int*)0);
        if (pid == -1)
            return;
        for (ALL) {
            if (p->pid == pid || p->pid == -1) {
                rmut(p);
                dfork(p);
            }
        }
    }
}

int main(void)
{
    setjmp(sjbuf);
    signal(SIGHUP, (void*)reset);
    for(ever) {
        shutdown();
        runcom();
        merge();
        multiple();
    }
}
