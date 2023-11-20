/* Header guard ------------------------------------------------------------- */

#ifndef __GUI_H__
#define __GUI_H__

/* Includes ----------------------------------------------------------------- */

#include <raylib.h>

#include "chip8.h"

/* Definitions -------------------------------------------------------------- */

#define GUI_WINDOW_WIDTH 1000
#define GUI_WINDOW_HEIGHT 560

#define GUI_MAIN_BACK_COLOR (WHITE)

#define GUI_GAME_BACK_COLOR (BLACK)
#define GUI_GAME_DRAW_COLOR (GREEN)

#define GUI_REGS_BACK_COLOR (BLACK)
#define GUI_REGS_FONT_COLOR (GREEN)

#define GUI_CODE_BACK_COLOR (BLACK) 

/* Data types --------------------------------------------------------------- */


/* Function prototypes ------------------------------------------------------ */

void gui_draw(chip8_t *ch8);

#endif /* __GUI_H__ */

/* -------------------------------------------------------------------------- */
