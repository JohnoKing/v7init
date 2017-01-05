/*
 * getty -- adapt to terminal speed on dialup, and call login
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <unistd.h>

#include "getty.h"

#define EOT 04 // EOT char
#define ERASE '#'
#define KILL '@'

static char name[16];
static int crmod;
static int upper;
static int lower;

static int getname(void)
{
    char cs;
    char *np;
    int c;

    crmod = 0;
    upper = 0;
    lower = 0;
    np = name;

    for (;;) {
        if (read(0, &cs, 1) <= 0)
            exit(0);
        if ((c = cs & 0177) == 0)
            return 0;
        if (c == EOT)
            exit(1);
        if (c == '\r' || c == '\n' || np >= &name[16])
            break;
        write(1, &cs, 1);
        if (c >= 'a' && c <= 'z')
            lower++;
        else if (c >= 'A' && c <= 'Z') {
            upper++;
            c += 'a' - 'A';
        } else if (c == ERASE) {
            if (np > name)
                np--;
            continue;
        } else if (c == KILL) {
            printf("\r\n");
            np = name;
            continue;
        } else if (c == ' ')
            c = '_';
        *np++ = (char)c;
    }

    *np = 0;
    if (c == '\r')
        crmod++;

    return 1;
}

int main(int argc, char *argv[])
{
    int tname = 0;

    if (argc > 1)
        tname = argv[1][0];

    switch (tname) {
    case '3': // Adapt to connect speed (212)
        ioctl(0, TIOCGETP, &tmode);
        if (tmode.sg_ispeed == B300)
            tname = '0';
        else
            tname = '3';
        break;
    }

    for (;;) {
        ioctl(0, TIOCSETP, &tmode);
        puts("login:");
        if (getname()) {
            tmode.sg_erase = ERASE;
            tmode.sg_kill = KILL;
            if (crmod)
                tmode.sg_flags |= CRMOD;
            if (upper)
                tmode.sg_flags |= LCASE;
            if (lower)
                tmode.sg_flags &= ~LCASE;
            ioctl(0, TIOCSETP, &tmode);
            printf("\n");
            execl("/bin/login", "login", name, 0);
            exit(1);
        }
    }
}
