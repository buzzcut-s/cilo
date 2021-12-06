#include <ctype.h>
#include <stdio.h>

#include <unistd.h>

#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();

    char c = 0;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
        if (iscntrl(c))
            printf("%d\n", c);
        else
            printf("%d ('%c')\n", c, c);
    }

    return 0;
}
