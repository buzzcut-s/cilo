#include <ctype.h>
#include <stdio.h>

#include <unistd.h>

#include <cilo/error.h>
#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();

    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1)
            die("read");

        if (iscntrl(c))
            printf("%d\r\n", c);
        else
            printf("%d ('%c')\r\n", c, c);

        if (c == 'q')
            break;
    }

    return 0;
}
