/*
 * getty.h
 */

struct sgetty
{
    char sg_ispeed; /* input speed */
    char sg_ospeed; /* output speed */
    char sg_erase; /* erase character */
    char sg_kill; /* kill character */
    int sg_flags; /* mode flags */
};

#define LCASE 04
#define CRMOD 020
#define B300 7

/*
 * some tty ioctl commands
 */
#define TIOCGETP (('t'<<8)|8)
#define TIOCSETP (('t'<<8)|9)
#define TIOCSETC (('t'<<8)|17)
