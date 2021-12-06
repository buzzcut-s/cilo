#include <cilo/editor.h>
#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();
    init_editor();

    while (1)
    {
        refresh_screen();
        process_keypress();
    }

    return 0;
}
