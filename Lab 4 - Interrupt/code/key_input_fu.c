/*
Program test_single_key.c
Get a key input without hitting Enter key
using termios
Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-
linux
Global old_termios and new_termios for efficient key inputs.
*/
#include <termios.h>
#include <stdio.h>
#include <unistd.h> // read()
#include <sys/select.h>
#include <errno.h>

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;
// Initialize new terminal i/o settings
void init_termios(int echo)
{
    tcgetattr(0, &old_tio);
    // Grab old_tio terminal i/o setting
    new_tio = old_tio;
    // Copy old_tio to new_tio
    new_tio.c_lflag &= ~ICANON;
    // disable buffered i/o
    new_tio.c_lflag &= echo? ECHO : ~ECHO; // Set echo mode
    if (tcsetattr(0, TCSANOW, &new_tio) < 0) perror("tcsetattr ~ICANON"); // Set new_tio terminal i/o setting
}
// Restore old terminal i/o settings
void reset_termios(void)
{
    tcsetattr(0, TCSANOW, &old_tio);
}
// Read one character without Enter key: Blocking
char getch(void)
{
    char ch = 0;
    if (read(0, &ch, 1) < 0)
        {
            if (errno == EINTR) return ch;
            perror ("read()");
        }
    return ch;
}
int key_hit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

