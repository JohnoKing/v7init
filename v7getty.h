/*
 * getty.h
 */

#define LCASE 04
#define CRMOD 020
#define B300 7
#define EOT 04 // EOT char
#define ERASE '#'
#define KILL '@'

// Some tty ioctl commands
#define TIOCGETP (('t' << 8) | 8)
#define TIOCSETP (('t' << 8) | 9)
#define TIOCSETC (('t' << 8) | 17)

struct sgetty {
    int sg_flags; // Mode flags
    char sg_ispeed; // Input speed
    char sg_ospeed; // Output speed
    char sg_erase; // Erase character
    char sg_kill; // Kill character
} static tmode;

static char name[16];
static int crmod;
static int upper;
static int lower;
