/* Includes ----------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chip8.h"

/* Function prototypes ------------------------------------------------------ */

static chip8_err_t __CLS(chip8_t *ch8);
static chip8_err_t __RET(chip8_t *ch8);
static chip8_err_t __SYS(chip8_t *ch8, uint16_t nnn);
static chip8_err_t __JP_NNN(chip8_t *ch8, uint16_t nnn);
static chip8_err_t __CALL_NNN(chip8_t *ch8, uint16_t nnn);
static chip8_err_t __SE_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn);
static chip8_err_t __SNE_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn);
static chip8_err_t __SE_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __LD_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn);
static chip8_err_t __ADD_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn);
static chip8_err_t __LD_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __OR_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __AND_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __XOR_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __ADD_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __SUB_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __SHR_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __SUBN_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __SHL_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __SNE_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y);
static chip8_err_t __LD_I_NNN(chip8_t *ch8, uint16_t nnn);
static chip8_err_t __JP_V0_NNN(chip8_t *ch8, uint16_t nnn);
static chip8_err_t __RND_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn);
static chip8_err_t __DRW_VX_VY_N(chip8_t *ch8, uint8_t x, uint8_t y, uint8_t n);
static chip8_err_t __SKP_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __SKNP_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_VX_DT(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_VX_K(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_DT_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_ST_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __ADD_I_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_F_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_B_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_I_VX(chip8_t *ch8, uint8_t x);
static chip8_err_t __LD_VX_I(chip8_t *ch8, uint8_t x);

/* Global variables --------------------------------------------------------- */

// Hex digits sprites
static const uint8_t fonts[FONT_SIZE * FONT_CHARS] = {
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

/* Function definitions ----------------------------------------------------- */

/**
 * @brief Initialize Chip8 interpreter instance
 *
 * @param ch8 Chip8 instance
 * @param cfg Chip8 configuration
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_init(chip8_t *ch8, chip8_cfg_t *cfg)
{
    srand(time(NULL));

    fseek(cfg->rom, 0, SEEK_END);
    uint16_t bytes = ftell(cfg->rom);
    rewind(cfg->rom);

    if (cfg->addr > CHIP8_RAM_END)
        return CHIP8_INVALID_ADDR;

    if (cfg->addr + bytes >= CHIP8_RAM_END)
        return CHIP8_INSUFF_MEMORY;

    memset(ch8, 0x0, sizeof(chip8_t));
    memcpy(&(ch8->ram[FONT_ADDR_START]), fonts, sizeof(fonts));
    fread(&(ch8->ram[cfg->addr]), sizeof(uint8_t), bytes, cfg->rom);

    ch8->PC = cfg->addr;

    return CHIP8_OK;
}

/**
 * @brief Run one Chip8 instruction cycle, must be called at 60 Hz
 *
 * @param ch8 Chip8 instance
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_run(chip8_t *ch8)
{
    // Fetch new instruction
    uint16_t PC = ch8->PC;
    uint16_t opcode = ((uint16_t)ch8->ram[PC] << 8) | ch8->ram[PC + 1];

    // Update program counter
    ch8->PC += sizeof(uint16_t);

    // Update timers
    if (ch8->DT > 0)
        --ch8->DT;
    if (ch8->ST > 0)
        --ch8->ST;

    // Parse opcode
    uint16_t O = (opcode & 0xF000);
    uint16_t NNN = (opcode & 0x0FFF);
    uint8_t NN = (opcode & 0x00FF);
    uint8_t N = (opcode & 0x000F);
    uint8_t X = (NNN >> 8);
    uint8_t Y = (NN >> 4);

    // Decode and execute
    switch (O)
    {
    case 0x0000:
        switch (NN)
        {
        case 0xE0:
            sprintf(ch8->decoded, "CLS");
            return __CLS(ch8);

        case 0xEE:
            sprintf(ch8->decoded, "RET");
            return __RET(ch8);

        default:
            sprintf(ch8->decoded, "SYS 0x%04X\t[ignored]", NNN);
            return __SYS(ch8, NNN);
        }
        break;

    case 0x1000:
        sprintf(ch8->decoded, "JP 0x%04X", NNN);
        return __JP_NNN(ch8, NNN);

    case 0x2000:
        sprintf(ch8->decoded, "CALL 0x%04X", NNN);
        return __CALL_NNN(ch8, NNN);

    case 0x3000:
        sprintf(ch8->decoded, "SE V%X, %d", X, NN);
        return __SE_VX_NN(ch8, X, NN);

    case 0x4000:
        sprintf(ch8->decoded, "SNE V%X, %d", X, NN);
        return __SNE_VX_NN(ch8, X, NN);

    case 0x5000:
        sprintf(ch8->decoded, "SE V%X, V%X", X, Y);
        return __SE_VX_VY(ch8, X, Y);

    case 0x6000:
        sprintf(ch8->decoded, "LD V%X, %d", X, NN);
        return __LD_VX_NN(ch8, X, NN);

    case 0x7000:
        sprintf(ch8->decoded, "ADD V%X, %d", X, NN);
        return __ADD_VX_NN(ch8, X, NN);

    case 0x8000:
        switch (N)
        {
        case 0x0:
            sprintf(ch8->decoded, "LD V%X, V%X", X, Y);
            return __LD_VX_VY(ch8, X, Y);

        case 0x1:
            sprintf(ch8->decoded, "OR V%X, V%X", X, Y);
            return __OR_VX_VY(ch8, X, Y);

        case 0x2:
            sprintf(ch8->decoded, "AND V%X, V%X", X, Y);
            return __AND_VX_VY(ch8, X, Y);

        case 0x3:
            sprintf(ch8->decoded, "XOR V%X, V%X", X, Y);
            return __XOR_VX_VY(ch8, X, Y);

        case 0x4:
            sprintf(ch8->decoded, "ADD, V%X, V%X\t[Set VF = carry]", X, Y);
            return __ADD_VX_VY(ch8, X, Y);

        case 0x5:
            sprintf(ch8->decoded, "SUB V%X, V%X\t[Set VF = NOT borrow]", X, Y);
            return __SUB_VX_VY(ch8, X, Y);

        case 0x6:
            sprintf(ch8->decoded, "SHR V%X, V%X\t[set VF = LSB]", X, Y);
            return __SHR_VX_VY(ch8, X, Y);

        case 0x7:
            sprintf(ch8->decoded, "SUBN V%X, V%X\t[set VF = NOT borrow]", X, Y);
            return __SUBN_VX_VY(ch8, X, Y);

        case 0xE:
            sprintf(ch8->decoded, "SHL V%X, V%X\t[set VF = MSB]", X, Y);
            return __SHL_VX_VY(ch8, X, Y);

        default:
            return CHIP8_INVALID_CMD;
        }
        break;

    case 0x9000:
        sprintf(ch8->decoded, "SNE V%X, V%X", X, Y);
        return __SNE_VX_VY(ch8, X, Y);

    case 0xA000:
        sprintf(ch8->decoded, "LD I, 0x%04X", NNN);
        return __LD_I_NNN(ch8, NNN);

    case 0xB000:
        sprintf(ch8->decoded, "JP V0, 0x%04X", NNN);
        return __JP_V0_NNN(ch8, NNN);

    case 0xC000:
        sprintf(ch8->decoded, "RND V%X, %d", X, NN);
        return __RND_VX_NN(ch8, X, NN);

    case 0xD000:
        sprintf(ch8->decoded, "DRW V%X, V%X, %d\t[set VF = collision]", X, Y, N);
        return __DRW_VX_VY_N(ch8, X, Y, N);

    case 0xE000:
        switch (NN)
        {
        case 0x9E:
            sprintf(ch8->decoded, "SKP V%X", X);
            return __SKP_VX(ch8, X);

        case 0xA1:
            sprintf(ch8->decoded, "SKNP V%X", X);
            return __SKNP_VX(ch8, X);

        default:
            return CHIP8_INVALID_CMD;
        }
        break;

    case 0xF000:
        switch (NN)
        {
        case 0x07:
            sprintf(ch8->decoded, "LD V%X, DT", X);
            return __LD_VX_DT(ch8, X);

        case 0x0A:
            sprintf(ch8->decoded, "LD V%X, K", X);
            return __LD_VX_K(ch8, X);

        case 0x15:
            sprintf(ch8->decoded, "LD DT, V%X", X);
            return __LD_DT_VX(ch8, X);

        case 0x18:
            sprintf(ch8->decoded, "LD ST, V%X", X);
            return __LD_ST_VX(ch8, X);

        case 0x1E:
            sprintf(ch8->decoded, "ADD I, V%X", X);
            return __ADD_I_VX(ch8, X);

        case 0x29:
            sprintf(ch8->decoded, "LD F, V%X", X);
            return __LD_F_VX(ch8, X);

        case 0x33:
            sprintf(ch8->decoded, "LD B, V%X", X);
            return __LD_B_VX(ch8, X);

        case 0x55:
            sprintf(ch8->decoded, "LD [I], V%X", X);
            return __LD_I_VX(ch8, X);

        case 0x65:
            sprintf(ch8->decoded, "LD V%X, [I]", X);
            return __LD_VX_I(ch8, X);

        default:
            return CHIP8_INVALID_CMD;
        }
        break;

    default:
        return CHIP8_INVALID_CMD;
    }

    return CHIP8_OK;
}

/**
 * @brief Clear the display
 * [00E0]
 *
 * @return chip8_err_t Error code
 */
chip8_err_t __CLS(chip8_t *ch8)
{
    memset(ch8->display, 0x0, sizeof(uint8_t) * DISPLAY_SIZE);
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
chip8_err_t __RET(chip8_t *ch8)
{
    if (ch8->SP == 0)
        return CHIP8_EXIT;

    ch8->PC = ch8->stack[ch8->SP--];
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
chip8_err_t __SYS(chip8_t *ch8, uint16_t nnn)
{
    (void)ch8;
    (void)nnn;

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
chip8_err_t __JP_NNN(chip8_t *ch8, uint16_t nnn)
{
    if (nnn > CHIP8_RAM_END)
        return CHIP8_INVALID_ADDR;

    ch8->PC = nnn;

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
chip8_err_t __CALL_NNN(chip8_t *ch8, uint16_t nnn)
{
    if (ch8->SP > STACK_SIZE - 1)
        return CHIP8_STACK_OVERFLOW;

    ch8->stack[++ch8->SP] = ch8->PC;
    ch8->PC = nnn;

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
chip8_err_t __SE_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn)
{
    if (ch8->V[x] == nn)
        ch8->PC += sizeof(uint16_t);

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
chip8_err_t __SNE_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn)
{
    if (ch8->V[x] != nn)
        ch8->PC += sizeof(uint16_t);

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
chip8_err_t __SE_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    if (ch8->V[x] == ch8->V[y])
        ch8->PC += sizeof(uint16_t);

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
chip8_err_t __LD_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn)
{
    ch8->V[x] = nn;

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
chip8_err_t __ADD_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn)
{
    ch8->V[x] += nn;

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
chip8_err_t __LD_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[x] = ch8->V[y];

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
chip8_err_t __OR_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[x] |= ch8->V[y];

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
chip8_err_t __AND_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[x] &= ch8->V[y];

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
chip8_err_t __XOR_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[x] ^= ch8->V[y];

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
chip8_err_t __ADD_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    uint16_t res = ch8->V[x] + ch8->V[y];
    ch8->V[0xF] = res >> 8;
    ch8->V[x] = (uint8_t)res;

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
chip8_err_t __SUB_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[0xF] = (ch8->V[x] > ch8->V[y]) ? 1 : 0;
    ch8->V[x] -= ch8->V[y];

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
chip8_err_t __SHR_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[0xF] = (ch8->V[x] & 0x1) ? 1 : 0;
    ch8->V[x] >>= ch8->V[y];

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
chip8_err_t __SUBN_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[0xF] = (ch8->V[y] > ch8->V[x]) ? 1 : 0;
    ch8->V[x] = ch8->V[y] - ch8->V[x];

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
chip8_err_t __SHL_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    ch8->V[0xF] = (ch8->V[x] & 0x64) ? 1 : 0;
    ch8->V[x] <<= ch8->V[y];

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
chip8_err_t __SNE_VX_VY(chip8_t *ch8, uint8_t x, uint8_t y)
{
    if (ch8->V[x] != ch8->V[y])
        ch8->PC += sizeof(uint16_t);

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
chip8_err_t __LD_I_NNN(chip8_t *ch8, uint16_t nnn)
{
    ch8->I = nnn;

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
chip8_err_t __JP_V0_NNN(chip8_t *ch8, uint16_t nnn)
{
    ch8->PC = ch8->V[0x0] + nnn;

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
chip8_err_t __RND_VX_NN(chip8_t *ch8, uint8_t x, uint8_t nn)
{
    uint8_t rnd = rand() % 256;
    ch8->V[x] = rnd & nn;

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
chip8_err_t __DRW_VX_VY_N(chip8_t *ch8, uint8_t x, uint8_t y, uint8_t n)
{
    uint8_t start_x = ch8->V[x] % DISPLAY_WIDTH;
    uint8_t start_y = ch8->V[y] % DISPLAY_HEIGHT;
    ch8->V[0xF] = 0;

    for (uint8_t h = 0; h < n; ++h)
    {
        uint8_t y = start_y + h;
        if (y >= DISPLAY_HEIGHT)
            break;

        for (uint8_t w = 0; w < SPRITE_WIDTH; ++w)
        {
            uint8_t x = start_x + w;
            if (x >= DISPLAY_WIDTH)
                continue;

            uint8_t old_val = ch8->display[x + y * DISPLAY_WIDTH];
            uint8_t new_val = old_val ^ (ch8->ram[ch8->I + h] & (1 << (7 - w)));

            ch8->display[x + y * DISPLAY_WIDTH] = new_val;
            ch8->V[0xF] |= ~new_val & old_val;
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
chip8_err_t __SKP_VX(chip8_t *ch8, uint8_t x)
{
    if (ch8->V[x] >= KEYS_SIZE)
        return CHIP8_INVALID_KEY;

    if (ch8->keys[ch8->V[x]])
        ch8->PC += sizeof(uint16_t);

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
chip8_err_t __SKNP_VX(chip8_t *ch8, uint8_t x)
{
    if (ch8->V[x] >= KEYS_SIZE)
        return CHIP8_INVALID_KEY;

    if (!ch8->keys[ch8->V[x]])
        ch8->PC += sizeof(uint16_t);

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
chip8_err_t __LD_VX_DT(chip8_t *ch8, uint8_t x)
{
    ch8->V[x] = ch8->DT;

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
chip8_err_t __LD_VX_K(chip8_t *ch8, uint8_t x)
{
    for (uint8_t i = 0; i < KEYS_SIZE; ++i)
    {
        if (ch8->keys[i] == 1)
        {
            ch8->V[x] = ch8->keys[i];
            return CHIP8_OK;
        }
    }

    ch8->PC -= sizeof(uint16_t);
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
chip8_err_t __LD_DT_VX(chip8_t *ch8, uint8_t x)
{
    ch8->DT = ch8->V[x];

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
chip8_err_t __LD_ST_VX(chip8_t *ch8, uint8_t x)
{
    ch8->ST = ch8->V[x];

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
chip8_err_t __ADD_I_VX(chip8_t *ch8, uint8_t x)
{
    ch8->I += ch8->V[x];

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
chip8_err_t __LD_F_VX(chip8_t *ch8, uint8_t x)
{
    uint8_t addr = (FONT_SIZE * ch8->V[x]) + FONT_ADDR_START;

    if (addr > FONT_ADDR_END)
        return CHIP8_INVALID_ADDR;

    ch8->I = addr;

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
chip8_err_t __LD_B_VX(chip8_t *ch8, uint8_t x)
{
    uint8_t val = ch8->V[x];

    for (uint8_t i = 3; i > 0; --i)
    {
        ch8->ram[ch8->I + i - 1] = val % 10;
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
chip8_err_t __LD_I_VX(chip8_t *ch8, uint8_t x)
{
    /** NOTE: Increment I instead of index in order to run COSMAC VIP games
     *  uint8_t index = ch8->I;
     */

    for (uint8_t i = 0; i <= x; ++i)
        ch8->ram[ch8->I++] = ch8->V[i];

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
chip8_err_t __LD_VX_I(chip8_t *ch8, uint8_t x)
{
    /** NOTE: Increment I instead of index in order to run COSMAC VIP games
     *  uint8_t index = ch8->I;
     */

    for (uint8_t i = 0; i <= x; ++i)
        ch8->V[i] = ch8->ram[ch8->I++];

    return CHIP8_OK;
}

/* -------------------------------------------------------------------------- */
