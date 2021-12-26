#include <cilo/editor/input.h>
#include <cilo/editor/state.h>
#include <cilo/file_io.h>
#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    enable_raw_mode();
    init_editor();

    if (argc >= 2)
        read_file(argv[1]);

    editor_set_help_message("HELP: Ctrl-Q = quit");

    while (1)
    {
        redraw_editor();
        process_keypress();
    }

    return 0;
}
