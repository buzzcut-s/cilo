#include <cilo/editor.h>
#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();

    while (1)
    {
        process_keypress();
    }

    return 0;
}
