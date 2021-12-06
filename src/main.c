#include <unistd.h>

#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();

    char c = 0;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
    }

    return 0;
}
