#ifndef COMMON_H
#define COMMON_H

#define ERASE_IN_DISPLAY_ALL "\x1b[2J"

#define ERASE_IN_LINE_TILL_END "\x1b[K"

#define DSR_ACTIVE_POSITION "\x1b[6n"

#define CURSOR_POSITION_1_1 "\x1b[H"

#define CURSOR_POSITION_Y_X "\x1b[%d;%dH"

#define SET_MODE_CURSOR_HIDE "\x1b[?25l"

#define SET_MODE_CURSOR_SHOW "\x1b[?25h"

#define CURSOR_F999_D999 "\x1b[999C\x1b[999B"

#endif  // COMMON_H
