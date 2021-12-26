#include <cilo/editor/input.h>
#include <cilo/editor/state.h>
#include <cilo/file_io.h>
#include <cilo/terminal.h>

int main(int argc, const char* argv[])
{
    terminal_enable_raw_mode();
    editor_state_init();

    if (argc >= 2)
        file_io_read(argv[1]);

    editor_state_set_help_message("HELP: Ctrl-Q = quit");

    while (1)
    {
        editor_state_redraw();
        editor_input_process();
    }

    return 0;
}
