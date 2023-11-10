/* Header guard ------------------------------------------------------------- */

#ifndef __CHIP8__
#define __CHIP8__

/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>

/* Definitions -------------------------------------------------------------- */

#define REGISTER_NUM 16
#define STACK_SIZE 16
#define RAM_SIZE 4096

#define DECODED_SIZE 50

#define FONT_CHARS 16
#define FONT_SIZE 5
#define FONT_ADDR_START (CHIP8_RAM_START)
#define FONT_ADDR_END (FONT_ADDR_START + (FONT_CHARS * FONT_SIZE))

#define COLOR_BLACK 0
#define COLOR_WHITE 1

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT_MIN 1
#define SPRITE_HEIGHT_MAX 15

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define KEYS_SIZE 16

/* Data types --------------------------------------------------------------- */

typedef enum
{
    CHIP8_OK,
    CHIP8_EXIT,
    CHIP8_INVALID_CMD,
    CHIP8_INVALID_KEY,
    CHIP8_INVALID_ADDR,
    CHIP8_INSUFF_MEMORY,
    CHIP8_STACK_OVERFLOW,
} chip8_err_t;

typedef enum
{
    CHIP8_RAM_START   = 0x000,
    CHIP8_RAM_PROGRAM = 0x200,
    CHIP8_RAM_ETI_660 = 0x600,
    CHIP8_RAM_END     = 0xFFF,
} chip8_ram_t;

typedef struct
{
    FILE *rom;
    chip8_ram_t addr;
} chip8_cfg_t;

typedef struct
{
    uint8_t V[REGISTER_NUM];       // General purpose registers
    uint8_t DT;                    // Delay timer register
    uint8_t ST;                    // Sound timer register
    uint16_t I;                    // Index register
    uint16_t PC;                   // Program counter register
    uint16_t SP;                   // Stack pointer register
    uint16_t stack[STACK_SIZE];    // Stack to store return addresses
    uint8_t ram[RAM_SIZE];         // RAM memory map
    uint8_t keys[KEYS_SIZE];       // IO keys states
    uint8_t display[DISPLAY_SIZE]; // Display buffer
    char decoded[DECODED_SIZE];    // Decoded opcode string size
} chip8_t;

/* Function prototypes ------------------------------------------------------ */

chip8_err_t chip8_init(chip8_t *ch8, chip8_cfg_t *cfg);
chip8_err_t chip8_run(chip8_t *ch8);

#endif /* __CHIP8__ */

/* -------------------------------------------------------------------------- */
