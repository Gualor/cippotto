#ifndef __CHIP8__
#define __CHIP8__

#include <stdint.h>

typedef enum
{
    CHIP8_OK,
    CHIP8_EXIT,
    CHIP8_UNKNOWN_CMD,
    CHIP8_INVALID_ADDR,
    CHIP8_STACK_OVERFLOW,
    CHIP8_ROM_LOAD_ERROR
} chip8_err_t;

chip8_err_t chip8_init(char *rom);
chip8_err_t chip8_fetch(uint16_t *opcode);
chip8_err_t chip8_execute(uint16_t opcode);

#endif /* __CHIP8__ */
