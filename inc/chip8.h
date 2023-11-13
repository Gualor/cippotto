/* Header guard ------------------------------------------------------------- */

#ifndef __CHIP8__
#define __CHIP8__

/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>

/* Definitions -------------------------------------------------------------- */

// Frequencies
#define CHIP8_CLOCK_HZ 500
#define CHIP8_TIMER_HZ 60

// Memory
#define CHIP8_REGS_NUM 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_RAM_SIZE 4096

// Display
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_DISPLAY_SIZE (CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT)

// Font
#define CHIP8_CHAR_NUM 16
#define CHIP8_CHAR_SIZE 5
#define CHIP8_FONT_SIZE (CHIP8_CHAR_NUM * CHIP8_CHAR_SIZE)
#define CHIP8_FONT_ADDR_START 0x000
#define CHIP8_FONT_ADDR_END (CHIP8_FONT_ADDR_START + CHIP8_FONT_SIZE)

// Keypad
#define CHIP8_KEYS_SIZE 16

// Opcode debug
#define CHIP8_DECODE_SIZE 50

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
    uint8_t V[CHIP8_REGS_NUM];           // General purpose registers
    uint8_t DT;                          // Delay timer register
    uint8_t ST;                          // Sound timer register
    uint16_t I;                          // Index register
    uint16_t PC;                         // Program counter register
    uint16_t SP;                         // Stack pointer register
    uint16_t stack[CHIP8_STACK_SIZE];    // Stack to store return addresses
    uint8_t ram[CHIP8_RAM_SIZE];         // RAM memory map
    uint8_t keys[CHIP8_KEYS_SIZE];       // IO keys states
    uint8_t display[CHIP8_DISPLAY_SIZE]; // Display buffer
    char decoded[CHIP8_DECODE_SIZE];     // Decoded opcode string size
    uint8_t pressed;                     // Key pressed flag
} chip8_t;

/* Function prototypes ------------------------------------------------------ */

chip8_err_t chip8_init(chip8_t *ch8, chip8_cfg_t *cfg);
chip8_err_t chip8_run(chip8_t *ch8);
chip8_err_t chip8_tick(chip8_t *ch8);

#endif /* __CHIP8__ */

/* -------------------------------------------------------------------------- */
