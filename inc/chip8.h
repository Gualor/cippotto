/* Header guard ------------------------------------------------------------- */

#ifndef __CHIP8__
#define __CHIP8__

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

/* Definitions -------------------------------------------------------------- */

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define KEYS_NUM 16

/* Data types --------------------------------------------------------------- */

typedef enum
{
    CHIP8_OK,
    CHIP8_EXIT,
    CHIP8_INVALID_CMD,
    CHIP8_INVALID_KEY,
    CHIP8_INVALID_ADDR,
    CHIP8_STACK_OVERFLOW,
    CHIP8_ROM_LOAD_ERROR,
    CHIP8_NOT_IMPLEMENTED,
} chip8_err_t;

typedef chip8_err_t (*chip8_cmd_t)(void);

/* Global variables --------------------------------------------------------- */

extern uint8_t chip8_display[DISPLAY_WIDTH][DISPLAY_HEIGHT];
extern uint8_t chip8_keys[KEYS_NUM];

/* Function prototypes ------------------------------------------------------ */

chip8_err_t chip8_init(char *rom);
chip8_err_t chip8_fetch(uint16_t *opcode);
chip8_err_t chip8_decode(chip8_cmd_t *cmd, uint16_t opcode);

#endif /* __CHIP8__ */

/* -------------------------------------------------------------------------- */
