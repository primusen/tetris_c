#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "helpers.h"

struct termios new_termios, orig_termios;

// Reset termios flags
void resetTerminalMode(void)
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

// Set termios flags for non canonical
void setConioTerminalMode(void)
{
    tcgetattr(0, &new_termios);

    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new_termios);
}

// Check if a key was pressed
int kbhit(void)
{
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

// Read a single character
int getchNonblockInput(void)
{
    if (kbhit())
    {
        int c = getchar();
        if (isalpha(c))
        {
            return tolower(c);
        }
    }
    return -1;
}
