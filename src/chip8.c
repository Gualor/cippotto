/* Includes ----------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chip8.h"

/* Definitions -------------------------------------------------------------- */

#define REGISTER_NUM 16
#define STACK_SIZE 16
#define RAM_SIZE 4096

#define FONT_CHARS 16
#define FONT_SIZE 5
#define FONT_ADDR_START (CHIP8_RAM_START)
#define FONT_ADDR_END (FONT_ADDR_START + (FONT_CHARS * FONT_SIZE))

#define COLOR_BLACK 0
#define COLOR_WHITE 1

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT_MIN 1
#define SPRITE_HEIGHT_MAX 15

/* Function prototypes ------------------------------------------------------ */

static chip8_err_t __CLS(void);
static chip8_err_t __RET(void);
static chip8_err_t __SYS(void);
static chip8_err_t __JP_NNN(void);
static chip8_err_t __CALL_NNN(void);
static chip8_err_t __SE_VX_NN(void);
static chip8_err_t __SNE_VX_NN(void);
static chip8_err_t __SE_VX_VY(void);
static chip8_err_t __LD_VX_NN(void);
static chip8_err_t __ADD_VX_NN(void);
static chip8_err_t __LD_VX_VY(void);
static chip8_err_t __OR_VX_VY(void);
static chip8_err_t __AND_VX_VY(void);
static chip8_err_t __XOR_VX_VY(void);
static chip8_err_t __ADD_VX_VY(void);
static chip8_err_t __SUB_VX_VY(void);
static chip8_err_t __SHR_VX_VY(void);
static chip8_err_t __SUBN_VX_VY(void);
static chip8_err_t __SHL_VX_VY(void);
static chip8_err_t __SNE_VX_VY(void);
static chip8_err_t __LD_I_NNN(void);
static chip8_err_t __JP_V0_NNN(void);
static chip8_err_t __RND_VX_NN(void);
static chip8_err_t __DRW_VX_VY_N(void);
static chip8_err_t __SKP_VX(void);
static chip8_err_t __SKNP_VX(void);
static chip8_err_t __LD_VX_DT(void);
static chip8_err_t __LD_VX_K(void);
static chip8_err_t __LD_DT_VX(void);
static chip8_err_t __LD_ST_VX(void);
static chip8_err_t __ADD_I_VX(void);
static chip8_err_t __LD_F_VX(void);
static chip8_err_t __LD_B_VX(void);
static chip8_err_t __LD_I_VX(void);
static chip8_err_t __LD_VX_I(void);

/* Global variables --------------------------------------------------------- */

// General purpose registers
static uint8_t V[REGISTER_NUM];

// Delay timer register
static uint8_t DT;

// Sound timer register
static uint8_t ST;

// Index register
static uint16_t I;

// Program counter register
static uint16_t PC;

// Stack pointer register
static uint16_t SP;

// Stack to store return addresses of subroutine calls
static uint16_t stack[STACK_SIZE];

// RAM memory map
static uint8_t ram[RAM_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

// Opcode parsing
static uint16_t O___;
static uint16_t _NNN;
static uint16_t __NN;
static uint16_t ___N;
static uint16_t X;
static uint16_t Y;

// Display buffer
uint8_t chip8_display[DISPLAY_WIDTH][DISPLAY_HEIGHT];

// Keyboard keys
uint8_t chip8_keys[KEYS_NUM];

/* Function definitions ----------------------------------------------------- */

/**
 * @brief CHIP8 interpreter initialization
 *
 * @param rom ROM path
 * @param prog_addr ROM path
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_init(char *rom, uint16_t prog_addr)
{
    srand(time(NULL));

    FILE *file = fopen(rom, "rb");
    if (file == NULL)
    {
        perror("Error opening the file");
        return CHIP8_ROM_LOAD_ERROR;
    }

    fseek(file, 0, SEEK_END);
    uint16_t prog_bytes = ftell(file);
    rewind(file);

    fread(&ram[prog_addr], sizeof(uint8_t), prog_bytes, file);
    fclose(file);

    memset(V, 0x0, sizeof(V));
    memset(stack, 0x0, sizeof(stack));
    memset(chip8_display, 0x0, sizeof(chip8_display));
    memset(chip8_keys, 0x0, sizeof(chip8_keys));

    I = 0;
    DT = 0;
    ST = 0;
    PC = prog_addr;
    SP = 0;

    return CHIP8_OK;
}

/**
 * @brief CHIP8 fetch new instruction
 *
 * @param opcode Fetched 16-bit opcode
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_fetch(uint16_t *opcode)
{
    *opcode = ((uint16_t)ram[PC] << 8) | ram[PC + 1];
    PC += sizeof(uint16_t);
    return CHIP8_OK;
}

/**
 * @brief CHIP8 decode opcode and retrieve command
 *
 * @param cmd Decoded command
 * @param opcode 16-bit opcode
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_decode(chip8_cmd_t *cmd, uint16_t opcode)
{
    chip8_err_t err = CHIP8_OK;
    *cmd = NULL;

    O___ = (opcode & 0xF000);
    _NNN = (opcode & 0x0FFF);
    __NN = (opcode & 0x00FF);
    ___N = (opcode & 0x000F);
    X = _NNN >> 8;
    Y = __NN >> 4;

    uint16_t addr = PC - sizeof(uint16_t);
    static char decoded[50] = "";

    switch (O___)
    {
    case 0x0000:
        switch (__NN)
        {
        case 0xE0:
            sprintf(decoded, "CLS");
            *cmd = __CLS;
            break;

        case 0xEE:
            sprintf(decoded, "RET");
            *cmd = __RET;
            break;

        default:
            sprintf(decoded, "SYS 0x%04X\t[ignored]", _NNN);
            *cmd = __SYS;
            break;
        }
        break;

    case 0x1000:
        sprintf(decoded, "JP 0x%04X", _NNN);
        *cmd = __JP_NNN;
        break;

    case 0x2000:
        sprintf(decoded, "CALL 0x%04X", _NNN);
        *cmd = __CALL_NNN;
        break;

    case 0x3000:
        sprintf(decoded, "SE V%X, %d", X, __NN);
        *cmd = __SE_VX_NN;
        break;

    case 0x4000:
        sprintf(decoded, "SNE V%X, %d", X, __NN);
        *cmd = __SNE_VX_NN;
        break;

    case 0x5000:
        sprintf(decoded, "SE V%X, V%X", X, Y);
        *cmd = __SE_VX_VY;
        break;

    case 0x6000:
        sprintf(decoded, "LD V%X, %d", X, __NN);
        *cmd = __LD_VX_NN;
        break;

    case 0x7000:
        sprintf(decoded, "ADD V%X, %d", X, __NN);
        *cmd = __ADD_VX_NN;
        break;

    case 0x8000:
        switch (___N)
        {
        case 0x0:
            sprintf(decoded, "LD V%X, V%X", X, Y);
            *cmd = __LD_VX_VY;
            break;

        case 0x1:
            sprintf(decoded, "OR V%X, V%X", X, Y);
            *cmd = __OR_VX_VY;
            break;

        case 0x2:
            sprintf(decoded, "AND V%X, V%X", X, Y);
            *cmd = __AND_VX_VY;
            break;

        case 0x3:
            sprintf(decoded, "XOR V%X, V%X", X, Y);
            *cmd = __XOR_VX_VY;
            break;

        case 0x4:
            sprintf(decoded, "ADD, V%X, V%X\t[Set VF = carry]", X, Y);
            *cmd = __ADD_VX_VY;
            break;

        case 0x5:
            sprintf(decoded, "SUB V%X, V%X\t[Set VF = NOT borrow]", X, Y);
            *cmd = __SUB_VX_VY;
            break;

        case 0x6:
            sprintf(decoded, "SHR V%X, V%X\t[set VF = LSB]", X, Y);
            *cmd = __SHR_VX_VY;
            break;

        case 0x7:
            sprintf(decoded, "SUBN V%X, V%X\t[set VF = NOT borrow]", X, Y);
            *cmd = __SUBN_VX_VY;
            break;

        case 0xE:
            sprintf(decoded, "SHL V%X, V%X\t[set VF = MSB]", X, Y);
            *cmd = __SHL_VX_VY;
            break;

        default:
            err = CHIP8_INVALID_CMD;
            break;
        }
        break;

    case 0x9000:
        sprintf(decoded, "SNE V%X, V%X", X, Y);
        *cmd = __SNE_VX_VY;
        break;

    case 0xA000:
        sprintf(decoded, "LD I, 0x%04X", _NNN);
        *cmd = __LD_I_NNN;
        break;

    case 0xB000:
        sprintf(decoded, "JP V0, 0x%04X", _NNN);
        *cmd = __JP_V0_NNN;
        break;

    case 0xC000:
        sprintf(decoded, "RND V%X, %d", X, __NN);
        *cmd = __RND_VX_NN;
        break;

    case 0xD000:
        sprintf(decoded, "DRW V%X, V%X, %d\t[set VF = collision]", X, Y, ___N);
        *cmd = __DRW_VX_VY_N;
        break;

    case 0xE000:
        switch (__NN)
        {
        case 0x9E:
            sprintf(decoded, "SKP V%X", X);
            *cmd = __SKP_VX;
            break;

        case 0xA1:
            sprintf(decoded, "SKNP V%X", X);
            *cmd = __SKNP_VX;
            break;

        default:
            err = CHIP8_INVALID_CMD;
            break;
        }
        break;

    case 0xF000:
        switch (__NN)
        {
        case 0x07:
            sprintf(decoded, "LD V%X, DT", X);
            *cmd = __LD_VX_DT;
            break;

        case 0x0A:
            sprintf(decoded, "LD V%X, K", X);
            *cmd = __LD_VX_K;
            break;

        case 0x15:
            sprintf(decoded, "LD DT, V%X", X);
            *cmd = __LD_DT_VX;
            break;

        case 0x18:
            sprintf(decoded, "LD ST, V%X", X);
            *cmd = __LD_ST_VX;
            break;

        case 0x1E:
            sprintf(decoded, "ADD I, V%X", X);
            *cmd = __ADD_I_VX;
            break;

        case 0x29:
            sprintf(decoded, "LD F, V%X", X);
            *cmd = __LD_F_VX;
            break;

        case 0x33:
            sprintf(decoded, "LD B, V%X", X);
            *cmd = __LD_B_VX;
            break;

        case 0x55:
            sprintf(decoded, "LD [I], V%X", X);
            *cmd = __LD_I_VX;
            break;

        case 0x65:
            sprintf(decoded, "LD V%X, [I]", X);
            *cmd = __LD_VX_I;
            break;

        default:
            err = CHIP8_INVALID_CMD;
            break;
        }
        break;

    default:
        err = CHIP8_INVALID_CMD;
        break;
    }

    // printf("0x%04X\t0x%04X\t%s\n", addr, opcode, decoded);

    return err;
}

/**
 * @brief CHIP8 decrement delay and sound timers, must be called at 60Hz
 * 
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_tick(void)
{
    if (DT > 0)
        --DT;
    if (ST > 0)
        --ST;

    return CHIP8_OK;
}

/**
 * @brief Clear the display
 * [00E0]
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __CLS(void)
{
    memset(chip8_display, 0x0, sizeof(chip8_display));
    return CHIP8_OK;
}

/**
 * @brief Return from a subroutine
 * [00EE]
 *
 * @details The interpreter sets the program counter to the address at the top
 * of the stack, then subtracts 1 from the stack pointer.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __RET(void)
{

    if (SP == 0)
        return CHIP8_EXIT;

    PC = stack[SP--];
    return CHIP8_OK;
}

/**
 * @brief Jump to a machine code routine at NNN
 * [0NNN]
 *
 * @details This instruction is only used on the old computers on which Chip-8
 * was originally implemented. It is ignored by modern interpreters.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SYS(void)
{
    return CHIP8_OK;
}

/**
 * @brief Jump to location NNN
 * [1NNN]
 *
 * @details The interpreter sets the program counter to NNN.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __JP_NNN(void)
{
    if (_NNN > CHIP8_RAM_END)
        return CHIP8_INVALID_ADDR;

    PC = _NNN;

    return CHIP8_OK;
}

/**
 * @brief Call subroutine at NNN
 * [2NNN]
 *
 * @details The interpreter increments the stack pointer, then puts the current
 * PC on the top of the stack. The PC is then set to NNN.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __CALL_NNN(void)
{
    if (SP > STACK_SIZE - 1)
        return CHIP8_STACK_OVERFLOW;

    stack[++SP] = PC;
    PC = _NNN;

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if VX = NN
 * [3XNN]
 *
 * @details The interpreter compares register VX to NN, and if they are equal,
 * increments the program counter by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SE_VX_NN(void)
{
    if (V[X] == __NN)
        PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if VX != NN
 * [4XNN]
 *
 * @details The interpreter compares register VX to NN, and if they are not
 * equal, increments the program counter by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SNE_VX_NN(void)
{
    if (V[X] != __NN)
        PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if VX = VY
 * [5XY0]
 *
 * @details The interpreter compares register VX to register VY, and if they
 * are equal, increments the program counter by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SE_VX_VY(void)
{
    if (V[X] == V[Y])
        PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Set VX = NN
 * [6XNN]
 *
 * @details The interpreter puts the value NN into register VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_VX_NN(void)
{
    V[X] = __NN;

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX + NN
 * [7XNN]
 *
 * Adds the value NN to the value of register VX, then stores the result in VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __ADD_VX_NN(void)
{
    V[X] += __NN;

    return CHIP8_OK;
}

/**
 * @brief Set VX = VY
 * [8XY0]
 *
 * @details Stores the value of register VY in register VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_VX_VY(void)
{
    V[X] = V[Y];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX OR VY
 * [8XY1]
 *
 * @details Performs a bitwise OR on the values of VX and VY, then stores the
 * result in VX. A bitwise OR compares the corrseponding bits from two values,
 * and if either bit is 1, then the same bit in the result is also 1.
 * Otherwise, it is 0.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __OR_VX_VY(void)
{
    V[X] |= V[Y];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX AND VY
 * [8XY2]
 *
 * @details Performs a bitwise AND on the values of VX and VY, then stores the
 * result in VX. A bitwise AND compares the corrseponding bits from two values,
 * and if both bits are 1, then the same bit in the result is also 1.
 * Otherwise, it is 0.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __AND_VX_VY(void)
{
    V[X] &= V[Y];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX XOR VY
 * [8XY3]
 *
 * @details Performs a bitwise exclusive OR on the values of VX and VY, then
 * stores the result in VX. An exclusive OR compares the corrseponding bits
 * from two values, and if the bits are not both the same, then the
 * corresponding bit in the result is set to 1. Otherwise, it is 0.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __XOR_VX_VY(void)
{
    V[X] ^= V[Y];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX + VY, set VF = carry
 * [8XY4]
 *
 * @details The values of VX and VY are added together. If the result is
 * greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the
 * lowest 8 bits of the result are kept, and stored in VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __ADD_VX_VY(void)
{
    uint16_t res = V[X] + V[Y];
    V[0xF] = res >> 8;
    V[X] = (uint8_t)res;

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX - VY, set VF = NOT borrow
 * [8XY5]
 *
 * @details If VX > VY, then VF is set to 1, otherwise 0. Then VY is subtracted
 * from VX, and the results stored in VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SUB_VX_VY(void)
{
    V[0xF] = (V[X] > V[Y]) ? 1 : 0;
    V[X] -= V[Y];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX SHR 1
 * [8XY6]
 *
 * @details If the least-significant bit of VX is 1, then VF is set to 1,
 * otherwise 0. Then VX is divided by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SHR_VX_VY(void)
{
    V[0xF] = (V[X] & 0x1) ? 1 : 0;
    V[X] >>= V[Y];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VY - VX, set VF = NOT borrow
 * [8XY7]
 *
 * @details If VY > VX, then VF is set to 1, otherwise 0. Then VX is subtracted
 * from VY, and the results stored in VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SUBN_VX_VY(void)
{
    V[0xF] = (V[Y] > V[X]) ? 1 : 0;
    V[X] = V[Y] - V[X];

    return CHIP8_OK;
}

/**
 * @brief Set VX = VX SHL 1
 * [8XYE]
 *
 * @details If the most-significant bit of VX is 1, then VF is set to 1,
 * otherwise to 0. Then VX is multiplied by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SHL_VX_VY(void)
{
    V[0xF] = (V[X] & 0x64) ? 1 : 0;
    V[X] <<= V[Y];

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if VX != VY
 * [9XY0]
 *
 * @details The values of VX and VY are compared, and if they are not equal,
 * the program counter is increased by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SNE_VX_VY(void)
{
    if (V[X] != V[Y])
        PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Set I = NNN
 * [ANNN]
 *
 * @details The value of register I is set to NNN.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_I_NNN(void)
{
    I = _NNN;

    return CHIP8_OK;
}

/**
 * @brief Jump to location NNN + V0
 * [BNNN]
 *
 * @details The program counter is set to NNN plus the value of V0.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __JP_V0_NNN(void)
{
    PC = V[0] + _NNN;

    return CHIP8_OK;
}

/**
 * @brief Set VX = random byte AND NN
 * [CXNN]
 *
 * @details The interpreter generates a random number from 0 to 255, which is
 * then ANDed with the value NN. The results are stored in VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __RND_VX_NN(void)
{
    uint8_t rnd = rand() % 256;
    V[X] = rnd & __NN;

    return CHIP8_OK;
}

/**
 * @brief Display N-byte sprite starting at memory location I at (VX, VY),
 * set VF = collision
 * [DXYN]
 *
 * @details The interpreter reads n bytes from memory, starting at the address
 * stored in I. These bytes are then displayed as sprites on screen at
 * coordinates (VX, VY). Sprites are XORed onto the existing screen. If this
 * causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the
 * display, it wraps around to the opposite side of the screen.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __DRW_VX_VY_N(void)
{
    uint8_t start_x = V[X] % DISPLAY_WIDTH;
    uint8_t start_y = V[Y] % DISPLAY_HEIGHT;
    V[0xF] = 0;

    for (uint8_t h = 0; h < ___N; ++h)
    {
        uint8_t y = start_y + h;
        if (y >= DISPLAY_HEIGHT)
            break;

        for (uint8_t w = 0; w < SPRITE_WIDTH; ++w)
        {
            uint8_t x = start_x + w;
            if (x >= DISPLAY_WIDTH)
                continue;

            uint8_t old_val = chip8_display[x][y];
            uint8_t new_val = old_val ^ (ram[I + h] & (1 << (7 - w)));

            chip8_display[x][y] = new_val;
            V[0xF] |= ~new_val & old_val;
        }
    }

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if key with the value of VX is pressed
 * [EX9E]
 *
 * @details Checks the keyboard, and if the key corresponding to the value of
 * VX is currently in the down position, PC is increased by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SKP_VX(void)
{
    if (V[X] >= KEYS_NUM)
        return CHIP8_INVALID_KEY;

    if (chip8_keys[V[X]])
        PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if key with the value of VX is not pressed
 * [EXA1]
 *
 * @details Checks the keyboard, and if the key corresponding to the value of
 * VX is currently in the up position, PC is increased by 2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __SKNP_VX(void)
{
    if (V[X] >= KEYS_NUM)
        return CHIP8_INVALID_KEY;

    if (!chip8_keys[V[X]])
        PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Set VX = delay timer value
 * [FX07]
 *
 * @details The value of DT is placed into VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_VX_DT(void)
{
    V[X] = DT;

    return CHIP8_OK;
}

/**
 * @brief Wait for a key press, store the value of the key in VX
 * [FX0A]
 *
 * @details All execution stops until a key is pressed, then the value of that
 * key is stored in VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_VX_K(void)
{
    for (uint8_t i = 0; i < KEYS_NUM; ++i)
    {
        if (chip8_keys[i] == 1)
        {
            V[X] = chip8_keys[i];
            return CHIP8_OK;
        }
    }

    PC -= sizeof(uint16_t);
    return CHIP8_OK;
}

/**
 * @brief Set delay timer = VX
 * [FX15]
 *
 * @details DT is set equal to the value of VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_DT_VX(void)
{
    DT = V[X];

    return CHIP8_OK;
}

/**
 * @brief Set sound timer = VX
 * [FX18]
 *
 * @details ST is set equal to the value of VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_ST_VX(void)
{
    ST = V[X];

    return CHIP8_OK;
}

/**
 * @brief Set I = I + VX
 * [FX1E]
 *
 * @details The values of I and VX are added, and the results are stored in I.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __ADD_I_VX(void)
{
    I += V[X];

    return CHIP8_OK;
}

/**
 * @brief Set I = location of sprite for digit VX
 * [FX29]
 *
 * @details The value of I is set to the location for the hexadecimal sprite
 * corresponding to the value of VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_F_VX(void)
{
    uint8_t addr = (FONT_SIZE * V[X]) + FONT_ADDR_START;

    if (addr > FONT_ADDR_END)
        return CHIP8_INVALID_ADDR;

    I = addr;

    return CHIP8_OK;
}

/**
 * @brief Store BCD representation of VX in memory locations I, I+1, and I+2
 * [FX33]
 *
 * @details The interpreter takes the decimal value of VX, and places the
 * hundreds digit in memory at location in I, the tens digit at location I+1,
 * and the ones digit at location I+2.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_B_VX(void)
{
    uint8_t val = V[X];

    for (uint8_t i = 0; i < 3; ++i)
    {
        ram[I + 2 - i] = val % 10;
        val /= 10;
    }

    return CHIP8_OK;
}

/**
 * @brief Store registers V0 through VX in memory starting at location I
 * [FX55]
 *
 * @details The interpreter copies the values of registers V0 through VX into
 * memory, starting at the address in I.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_I_VX(void)
{
    /** NOTE: Increment I instead of index in order to run COSMAC VIP games */
    uint8_t index = I;

    for (uint8_t i = 0; i <= X; ++i)
        ram[index++] = V[i];

    return CHIP8_OK;
}

/**
 * @brief Read registers V0 through VX from memory starting at location I
 * [FX65]
 *
 * @details The interpreter reads values from memory starting at location I
 * into registers V0 through VX.
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __LD_VX_I(void)
{
    /** NOTE: Increment I instead of index in order to run COSMAC VIP games */
    uint8_t index = I;

    for (uint8_t i = 0; i <= X; ++i)
        V[i] = ram[index++];

    return CHIP8_OK;
}

/* -------------------------------------------------------------------------- */
