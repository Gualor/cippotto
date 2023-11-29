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
#define CHIP8_CLOCK_TIMER_RATIO ((CHIP8_CLOCK_HZ / CHIP8_TIMER_HZ) + 1)

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

// Decode string
#define CHIP8_DECODE_STR_SIZE 50

/* Data types --------------------------------------------------------------- */

/**
 * @brief Chip-8 RAM addresses
 *
 */
typedef enum
{
    CHIP8_RAM_START   = 0x000,
    CHIP8_RAM_PROGRAM = 0x200,
    CHIP8_RAM_ETI_660 = 0x600,
    CHIP8_RAM_END     = 0xFFF,
} chip8_ram_t;

/**
 * @brief Chip-8 configurations
 *
 */
typedef struct
{
    FILE *rom;
    chip8_ram_t addr;
} chip8_cfg_t;

/**
 * @brief Chip-8 data structure
 *
 */
typedef struct
{
    /* Registers */
    uint8_t V[CHIP8_REGS_NUM];           // General purpose registers
    uint8_t DT;                          // Delay timer register
    uint8_t ST;                          // Sound timer register
    uint16_t I;                          // Index register
    uint16_t PC;                         // Program counter register
    uint16_t SP;                         // Stack pointer register
    /* Stack */
    uint16_t stack[CHIP8_STACK_SIZE];    // Stack to store return addresses
    /* RAM */
    uint8_t ram[CHIP8_RAM_SIZE];         // RAM memory (program + data)
    /* IO */
    uint32_t cycles;                     // CPU cycles counter
    uint8_t pressed;                     // Key pressed flag
    uint8_t keys[CHIP8_KEYS_SIZE];       // Keys states
    uint8_t display[CHIP8_DISPLAY_SIZE]; // Display buffer
} chip8_t;

/**
 * @brief Chip-8 parsed opcode
 *
 */
typedef struct
{
    uint16_t o   : 16;
    uint16_t nnn : 12;
    uint8_t nn   : 8;
    uint8_t n    : 4;
    uint8_t x    : 4;
    uint8_t y    : 4;
} chip8_op_t;

/**
 * @brief Chip-8 API error code
 *
 */
typedef enum
{
    CHIP8_OK             = 0x0,
    CHIP8_EXIT           = 0xA,
    CHIP8_INVALID_CMD    = 0xB,
    CHIP8_INVALID_KEY    = 0xC,
    CHIP8_INVALID_ADDR   = 0xD,
    CHIP8_INSUFF_MEMORY  = 0xE,
    CHIP8_STACK_OVERFLOW = 0xF,
} chip8_err_t;

/**
 * @brief Chip-8 command pointer
 *
 */
typedef chip8_err_t (*chip8_cmd_t)(chip8_t *, chip8_op_t);

/* Function prototypes ------------------------------------------------------ */

chip8_err_t chip8_init(chip8_t *ch8, chip8_cfg_t cfg);
chip8_err_t chip8_run(chip8_t *ch8);
chip8_err_t chip8_fetch(chip8_t *ch8, chip8_op_t *op);
chip8_err_t chip8_decode(chip8_cmd_t *cmd, char *str, chip8_op_t op);
chip8_err_t chip8_execute(chip8_t *ch8, chip8_cmd_t cmd, chip8_op_t op);

#endif /* __CHIP8__ */

/* -------------------------------------------------------------------------- */
