#include <cilo/editor/state.h>
#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();
    init_editor();

    while (1)
    {
        redraw_editor();
        process_keypress();
    }

    return 0;
}
