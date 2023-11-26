/* Includes ----------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chip8.h"

/* Function prototypes ------------------------------------------------------ */

static chip8_err_t __CLS(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __RET(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SYS(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __JP_NNN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __CALL_NNN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SE_VX_NN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SNE_VX_NN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SE_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_VX_NN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __ADD_VX_NN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __OR_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __AND_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __XOR_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __ADD_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SUB_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SHR_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SUBN_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SHL_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SNE_VX_VY(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_I_NNN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __JP_V0_NNN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __RND_VX_NN(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __DRW_VX_VY_N(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SKP_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __SKNP_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_VX_DT(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_VX_K(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_DT_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_ST_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __ADD_I_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_F_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_B_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_I_VX(chip8_t *ch8, chip8_op_t op);
static chip8_err_t __LD_VX_I(chip8_t *ch8, chip8_op_t op);

/* Global variables --------------------------------------------------------- */

static const uint8_t chip8_fonts[CHIP8_FONT_SIZE] = {
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
 * @brief Initialize Chip-8 interpreter instance
 *
 * @param ch8 Chip-8 instance
 * @param cfg Chip-8 configuration
 * @return    Error code
 */
chip8_err_t chip8_init(chip8_t *ch8, chip8_cfg_t cfg)
{
    srand(time(NULL));

    fseek(cfg.rom, 0, SEEK_END);
    uint16_t bytes = ftell(cfg.rom);
    rewind(cfg.rom);

    if (cfg.addr > CHIP8_RAM_END)
        return CHIP8_INVALID_ADDR;

    if (cfg.addr + bytes >= CHIP8_RAM_END)
        return CHIP8_INSUFF_MEMORY;

    memset(ch8, 0x0, sizeof(chip8_t));
    memcpy(&ch8->ram[CHIP8_FONT_ADDR_START], chip8_fonts, sizeof(chip8_fonts));
    fread(&ch8->ram[cfg.addr], sizeof(uint8_t), bytes, cfg.rom);

    ch8->PC = cfg.addr;

    return CHIP8_OK;
}

/**
 * @brief Run one Chip-8 CPU cycle (fetch-decode-execute)
 * 
 * @param ch8 Chip-8 instance
 * @return    Error code 
 */
chip8_err_t chip8_run(chip8_t *ch8)
{
    chip8_err_t err;
    chip8_op_t op;
    chip8_cmd_t cmd;

    err = chip8_fetch(ch8, &op);
    if (err)
        return err;

    err = chip8_decode(&cmd, NULL, op);
    if (err)
        return err;

    err = chip8_execute(ch8, cmd, op);
    if (err)
        return err;

    return CHIP8_OK;
}

/**
 * @brief Parse a Chip-8 opcode
 * 
 * @param op  Parsed opcode
 * @param raw Raw opcode
 * @return    Error code
 */
chip8_err_t chip8_parse(chip8_op_t *op, uint16_t raw)
{
    op->o = (raw & 0xF000);
    op->nnn = (raw & 0x0FFF);
    op->nn = (raw & 0x00FF);
    op->n = (raw & 0x000F);
    op->x = (op->nnn >> 8);
    op->y = (op->nn >> 4);

    return CHIP8_OK;
}

/**
 * @brief Fetch new Chip-8 instruction
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t chip8_fetch(chip8_t *ch8, chip8_op_t *op)
{
    // Fetch new instruction
    uint16_t raw = ((uint16_t)ch8->ram[ch8->PC] << 8) | ch8->ram[ch8->PC + 1];

    // Parse opcode
    chip8_parse(op, raw);

    // Update program counter
    ch8->PC += sizeof(uint16_t);

    // Update timers
    if ((ch8->cycles++ % CHIP8_CLOCK_TIMER_RATIO) == 0)
    {
        if (ch8->DT > 0)
            --ch8->DT;
        if (ch8->ST > 0)
            --ch8->ST;
    }

    return CHIP8_OK;
}

/**
 * @brief Decode a Chip-8 opcode
 *
 * @param cmd Decoded command
 * @param str Decoded string
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t chip8_decode(chip8_cmd_t *cmd, char *str, chip8_op_t op)
{
    static chip8_err_t err = CHIP8_OK;
    static chip8_cmd_t _cmd = NULL;
    static char _str[CHIP8_DECODE_STR_SIZE] = "UNKNOWN";

    switch (op.o)
    {
    case 0x0000:
        switch (op.nn)
        {
        case 0xE0:
            sprintf(_str, "CLS");
            _cmd = __CLS;
            break;

        case 0xEE:
            sprintf(_str, "RET");
            _cmd = __RET;
            break;

        default:
            sprintf(_str, "SYS 0x%04X", op.nnn);
            _cmd = __SYS;
            break;
        }
        break;

    case 0x1000:
        sprintf(_str, "JP 0x%04X", op.nnn);
        _cmd = __JP_NNN;
        break;

    case 0x2000:
        sprintf(_str, "CALL 0x%04X", op.nnn);
        _cmd = __CALL_NNN;
        break;

    case 0x3000:
        sprintf(_str, "SE V%X, %d", op.x, op.nn);
        _cmd = __SE_VX_NN;
        break;

    case 0x4000:
        sprintf(_str, "SNE V%X, %d", op.x, op.nn);
        _cmd = __SNE_VX_NN;
        break;

    case 0x5000:
        sprintf(_str, "SE V%X, V%X", op.x, op.y);
        _cmd = __SE_VX_VY;
        break;

    case 0x6000:
        sprintf(_str, "LD V%X, %d", op.x, op.nn);
        _cmd = __LD_VX_NN;
        break;

    case 0x7000:
        sprintf(_str, "ADD V%X, %d", op.x, op.nn);
        _cmd = __ADD_VX_NN;
        break;

    case 0x8000:
        switch (op.n)
        {
        case 0x0:
            sprintf(_str, "LD V%X, V%X", op.x, op.y);
            _cmd = __LD_VX_VY;
            break;

        case 0x1:
            sprintf(_str, "OR V%X, V%X", op.x, op.y);
            _cmd = __OR_VX_VY;
            break;

        case 0x2:
            sprintf(_str, "AND V%X, V%X", op.x, op.y);
            _cmd = __AND_VX_VY;
            break;

        case 0x3:
            sprintf(_str, "XOR V%X, V%X", op.x, op.y);
            _cmd = __XOR_VX_VY;
            break;

        case 0x4:
            sprintf(_str, "ADD, V%X, V%X", op.x, op.y);
            _cmd = __ADD_VX_VY;
            break;

        case 0x5:
            sprintf(_str, "SUB V%X, V%X", op.x, op.y);
            _cmd = __SUB_VX_VY;
            break;

        case 0x6:
            sprintf(_str, "SHR V%X, V%X", op.x, op.y);
            _cmd = __SHR_VX_VY;
            break;

        case 0x7:
            sprintf(_str, "SUBN V%X, V%X", op.x, op.y);
            _cmd = __SUBN_VX_VY;
            break;

        case 0xE:
            sprintf(_str, "SHL V%X, V%X", op.x, op.y);
            _cmd = __SHL_VX_VY;
            break;

        default:
            err = CHIP8_INVALID_CMD;
            break;
        }
        break;

    case 0x9000:
        sprintf(_str, "SNE V%X, V%X", op.x, op.y);
        _cmd = __SNE_VX_VY;
        break;

    case 0xA000:
        sprintf(_str, "LD I, 0x%04X", op.nnn);
        _cmd = __LD_I_NNN;
        break;

    case 0xB000:
        sprintf(_str, "JP V0, 0x%04X", op.nnn);
        _cmd = __JP_V0_NNN;
        break;

    case 0xC000:
        sprintf(_str, "RND V%X, %d", op.x, op.nn);
        _cmd = __RND_VX_NN;
        break;

    case 0xD000:
        sprintf(_str, "DRW V%X, V%X, %d", op.x, op.y, op.n);
        _cmd = __DRW_VX_VY_N;
        break;

    case 0xE000:
        switch (op.nn)
        {
        case 0x9E:
            sprintf(_str, "SKP V%X", op.x);
            _cmd = __SKP_VX;
            break;

        case 0xA1:
            sprintf(_str, "SKNP V%X", op.x);
            _cmd = __SKNP_VX;
            break;

        default:
            err = CHIP8_INVALID_CMD;
            break;
        }
        break;

    case 0xF000:
        switch (op.nn)
        {
        case 0x07:
            sprintf(_str, "LD V%X, DT", op.x);
            _cmd = __LD_VX_DT;
            break;

        case 0x0A:
            sprintf(_str, "LD V%X, K", op.x);
            _cmd = __LD_VX_K;
            break;

        case 0x15:
            sprintf(_str, "LD DT, V%X", op.x);
            _cmd = __LD_DT_VX;
            break;

        case 0x18:
            sprintf(_str, "LD ST, V%X", op.x);
            _cmd = __LD_ST_VX;
            break;

        case 0x1E:
            sprintf(_str, "ADD I, V%X", op.x);
            _cmd = __ADD_I_VX;
            break;

        case 0x29:
            sprintf(_str, "LD F, V%X", op.x);
            _cmd = __LD_F_VX;
            break;

        case 0x33:
            sprintf(_str, "LD B, V%X", op.x);
            _cmd = __LD_B_VX;
            break;

        case 0x55:
            sprintf(_str, "LD [I], V%X", op.x);
            _cmd = __LD_I_VX;
            break;

        case 0x65:
            sprintf(_str, "LD V%X, [I]", op.x);
            _cmd = __LD_VX_I;
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

    if (cmd != NULL)
        memcpy(cmd, &_cmd, sizeof(chip8_cmd_t));

    if (str != NULL)
        strcpy(str, _str);

    return err;
}

/**
 * @brief Execute a Chip-8 command
 *
 * @param ch8 Chip-8 instance
 * @param cmd Command to execute
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t chip8_execute(chip8_t *ch8, chip8_cmd_t cmd, chip8_op_t op)
{
    return cmd(ch8, op);
}

/**
 * @brief Clear the display
 * [00E0]
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __CLS(chip8_t *ch8, chip8_op_t op)
{
    (void)op;
    memset(ch8->display, 0x0, sizeof(uint8_t) * CHIP8_DISPLAY_SIZE);

    return CHIP8_OK;
}

/**
 * @brief Return from a subroutine
 * [00EE]
 *
 * @details The interpreter sets the program counter to the address at the top
 * of the stack, then subtracts 1 from the stack pointer.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __RET(chip8_t *ch8, chip8_op_t op)
{
    (void)op;

    if (ch8->SP == 0)
        return CHIP8_EXIT;

    ch8->PC = ch8->stack[ch8->SP--];
    return CHIP8_OK;
}

/**
 * @brief Jump to a machine code routine at nnn
 * [0NNN]
 *
 * @details This instruction is only used on the old computers on which Chip-8
 * was originally implemented. It is ignored by modern interpreters.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SYS(chip8_t *ch8, chip8_op_t op)
{
    (void)ch8;
    (void)op;

    return CHIP8_OK;
}

/**
 * @brief Jump to location NNN
 * [1NNN]
 *
 * @details The interpreter sets the program counter to nnn.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __JP_NNN(chip8_t *ch8, chip8_op_t op)
{
    if (op.nnn > CHIP8_RAM_END)
        return CHIP8_INVALID_ADDR;

    ch8->PC = op.nnn;

    return CHIP8_OK;
}

/**
 * @brief Call subroutine at nnn
 * [2NNN]
 *
 * @details The interpreter increments the stack pointer, then puts the current
 * PC on the top of the stack. The PC is then set to nnn.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __CALL_NNN(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->SP > CHIP8_STACK_SIZE - 1)
        return CHIP8_STACK_OVERFLOW;

    ch8->stack[++ch8->SP] = ch8->PC;
    ch8->PC = op.nnn;

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if Vx = nn
 * [3XNN]
 *
 * @details The interpreter compares register Vx to nn, and if they are equal,
 * increments the program counter by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SE_VX_NN(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->V[op.x] == op.nn)
        ch8->PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if Vx != nn
 * [4XNN]
 *
 * @details The interpreter compares register Vx to nn, and if they are not
 * equal, increments the program counter by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SNE_VX_NN(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->V[op.x] != op.nn)
        ch8->PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if Vx = Vy
 * [5XY0]
 *
 * @details The interpreter compares register Vx to register Vy, and if they
 * are equal, increments the program counter by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SE_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->V[op.x] == ch8->V[op.y])
        ch8->PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Set Vx = nn
 * [6XNN]
 *
 * @details The interpreter puts the value nn into register Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_VX_NN(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] = op.nn;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vx + nn
 * [7XNN]
 *
 * @details Adds the value nn to the value of register Vx, then stores the
 * result in Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __ADD_VX_NN(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] += op.nn;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vy
 * [8XY0]
 *
 * @details Stores the value of register Vy in register Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] = ch8->V[op.y];

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vx OR Vx
 * [8XY1]
 *
 * @details Performs a bitwise OR on the values of Vx and Vx, then stores the
 * result in Vx. A bitwise OR compares the corrseponding bits from two values,
 * and if either bit is 1, then the same bit in the result is also 1.
 * Otherwise, it is 0.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __OR_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] |= ch8->V[op.y];
    ch8->V[0xF] = 0;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vx AND Vy
 * [8XY2]
 *
 * @details Performs a bitwise AND on the values of Vx and Vy, then stores the
 * result in Vx. A bitwise AND compares the corrseponding bits from two values,
 * and if both bits are 1, then the same bit in the result is also 1.
 * Otherwise, it is 0.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __AND_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] &= ch8->V[op.y];
    ch8->V[0xF] = 0;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vx XOR Vy
 * [8XY3]
 *
 * @details Performs a bitwise exclusive OR on the values of Vx and Vy, then
 * stores the result in Vx. An exclusive OR compares the corrseponding bits
 * from two values, and if the bits are not both the same, then the
 * corresponding bit in the result is set to 1. Otherwise, it is 0.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __XOR_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] ^= ch8->V[op.y];
    ch8->V[0xF] = 0;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vx + Vy, set VF = carry
 * [8XY4]
 *
 * @details The values of Vx and Vy are added together. If the result is
 * greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the
 * lowest 8 bits of the result are kept, and stored in Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __ADD_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    uint16_t res = ch8->V[op.x] + ch8->V[op.y];
    ch8->V[op.x] = (uint8_t)res;
    ch8->V[0xF] = (uint8_t)(res >> 8);

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vx - Vy, set VF = NOT borrow
 * [8XY5]
 *
 * @details If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted
 * from Vx, and the results stored in Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SUB_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    uint8_t flag = ch8->V[op.x] >= ch8->V[op.y];
    ch8->V[op.x] -= ch8->V[op.y];
    ch8->V[0xF] = flag;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vy SHR 1
 * [8XY6]
 *
 * @details If the least-significant bit of Vx is 1, then VF is set to 1,
 * otherwise 0. Then Vx is divided by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SHR_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    uint8_t flag = ch8->V[op.y] & 0x1;
    ch8->V[op.x] = ch8->V[op.y] >> 1;
    ch8->V[0xF] = flag;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vy - Vx, set VF = NOT borrow
 * [8XY7]
 *
 * @details If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted
 * from Vy, and the results stored in Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SUBN_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    uint8_t flag = ch8->V[op.y] >= ch8->V[op.x];
    ch8->V[op.x] = ch8->V[op.y] - ch8->V[op.x];
    ch8->V[0xF] = flag;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = Vy SHL 1
 * [8XYE]
 *
 * @details If the most-significant bit of Vx is 1, then VF is set to 1,
 * otherwise to 0. Then Vx is multiplied by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SHL_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    uint8_t flag = (ch8->V[op.y] & 0x80) >> 7;
    ch8->V[op.x] = ch8->V[op.y] << 1;
    ch8->V[0xF] = flag;

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if Vx != Vy
 * [9XY0]
 *
 * @details The values of Vx and Vy are compared, and if they are not equal,
 * the program counter is increased by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SNE_VX_VY(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->V[op.x] != ch8->V[op.y])
        ch8->PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Set I = nnn
 * [ANNN]
 *
 * @details The value of register I is set to nnn.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_I_NNN(chip8_t *ch8, chip8_op_t op)
{
    ch8->I = op.nnn;

    return CHIP8_OK;
}

/**
 * @brief Jump to location nnn + V0
 * [BNNN]
 *
 * @details The program counter is set to nnn plus the value of V0.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __JP_V0_NNN(chip8_t *ch8, chip8_op_t op)
{
    ch8->PC = ch8->V[0x0] + op.nnn;

    return CHIP8_OK;
}

/**
 * @brief Set Vx = random byte AND nn
 * [CXNN]
 *
 * @details The interpreter generates a random number from 0 to 255, which is
 * then ANDed with the value nn. The results are stored in Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __RND_VX_NN(chip8_t *ch8, chip8_op_t op)
{
    uint8_t rnd = rand() % 256;
    ch8->V[op.x] = rnd & op.nn;

    return CHIP8_OK;
}

/**
 * @brief Display n-byte sprite starting at memory location I at (Vx, Vy),
 * set VF = collision
 * [DXYN]
 *
 * @details The interpreter reads n bytes from memory, starting at the address
 * stored in I. These bytes are then displayed as sprites on screen at
 * coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this
 * causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the
 * display, it wraps around to the opposite side of the screen.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __DRW_VX_VY_N(chip8_t *ch8, chip8_op_t op)
{
    uint8_t flag = 0;
    uint8_t start_x = ch8->V[op.x] % CHIP8_DISPLAY_WIDTH;
    uint8_t start_y = ch8->V[op.y] % CHIP8_DISPLAY_HEIGHT;

    for (uint8_t h = 0; h < op.n; ++h)
    {
        uint8_t sprite = ch8->ram[ch8->I + h];

        uint16_t y = start_y + h;
        if (y >= CHIP8_DISPLAY_HEIGHT)
            break;

        for (uint8_t w = 0; w < 8; ++w)
        {
            uint16_t x = start_x + w;
            if (x >= CHIP8_DISPLAY_WIDTH)
                continue;

            uint16_t offset = x + (y * CHIP8_DISPLAY_WIDTH);
            uint8_t old_val = ch8->display[offset];
            uint8_t new_val = old_val ^ ((sprite & 0x80) >> 7);

            ch8->display[offset] = new_val;
            flag |= ~new_val & old_val;

            sprite <<= 1;
        }
    }

    ch8->V[0xF] = flag;

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if key with the value of Vx is pressed
 * [EX9E]
 *
 * @details Checks the keyboard, and if the key corresponding to the value of
 * Vx is currently in the down position, PC is increased by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SKP_VX(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->V[op.x] >= CHIP8_KEYS_SIZE)
        return CHIP8_INVALID_KEY;

    if (ch8->keys[ch8->V[op.x]])
        ch8->PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Skip next instruction if key with the value of Vx is not pressed
 * [EXA1]
 *
 * @details Checks the keyboard, and if the key corresponding to the value of
 * Vx is currently in the up position, PC is increased by 2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __SKNP_VX(chip8_t *ch8, chip8_op_t op)
{
    if (ch8->V[op.x] >= CHIP8_KEYS_SIZE)
        return CHIP8_INVALID_KEY;

    if (!ch8->keys[ch8->V[op.x]])
        ch8->PC += sizeof(uint16_t);

    return CHIP8_OK;
}

/**
 * @brief Set Vx = delay timer value
 * [FX07]
 *
 * @details The value of DT is placed into Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_VX_DT(chip8_t *ch8, chip8_op_t op)
{
    ch8->V[op.x] = ch8->DT;

    return CHIP8_OK;
}

/**
 * @brief Wait for a key press, store the value of the key in Vx
 * [FX0A]
 *
 * @details All execution stops until a key is pressed, then the value of that
 * key is stored in Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_VX_K(chip8_t *ch8, chip8_op_t op)
{
    uint8_t pressed = 0;

    for (uint8_t i = 0; i < CHIP8_KEYS_SIZE; ++i)
    {
        if (ch8->keys[i])
        {
            ch8->V[op.x] = ch8->keys[i];
            pressed = 1;
            break;
        }
    }

    if (!pressed && ch8->pressed)
    {
        ch8->pressed = 0;
    }
    else
    {
        ch8->pressed = pressed;
        ch8->PC -= sizeof(uint16_t);
    }

    return CHIP8_OK;
}

/**
 * @brief Set delay timer = Vx
 * [FX15]
 *
 * @details DT is set equal to the value of Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_DT_VX(chip8_t *ch8, chip8_op_t op)
{
    ch8->DT = ch8->V[op.x];

    return CHIP8_OK;
}

/**
 * @brief Set sound timer = Vx
 * [FX18]
 *
 * @details ST is set equal to the value of Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_ST_VX(chip8_t *ch8, chip8_op_t op)
{
    ch8->ST = ch8->V[op.x];

    return CHIP8_OK;
}

/**
 * @brief Set I = I + Vx
 * [FX1E]
 *
 * @details The values of I and Vx are added, and the results are stored in I.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __ADD_I_VX(chip8_t *ch8, chip8_op_t op)
{
    ch8->I += ch8->V[op.x];

    return CHIP8_OK;
}

/**
 * @brief Set I = location of sprite for digit Vx
 * [FX29]
 *
 * @details The value of I is set to the location for the hexadecimal sprite
 * corresponding to the value of Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_F_VX(chip8_t *ch8, chip8_op_t op)
{
    uint8_t addr = (CHIP8_CHAR_SIZE * ch8->V[op.x]) + CHIP8_FONT_ADDR_START;

    if (addr > CHIP8_FONT_ADDR_END)
        return CHIP8_INVALID_ADDR;

    ch8->I = addr;

    return CHIP8_OK;
}

/**
 * @brief Store BCD representation of Vx in memory locations I, I+1, and I+2
 * [FX33]
 *
 * @details The interpreter takes the decimal value of Vx, and places the
 * hundreds digit in memory at location in I, the tens digit at location I+1,
 * and the ones digit at location I+2.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_B_VX(chip8_t *ch8, chip8_op_t op)
{
    uint8_t val = ch8->V[op.x];

    for (uint8_t i = 3; i > 0; --i)
    {
        ch8->ram[ch8->I + i - 1] = val % 10;
        val /= 10;
    }

    return CHIP8_OK;
}

/**
 * @brief Store registers V0 through Vx in memory starting at location I
 * [FX55]
 *
 * @details The interpreter copies the values of registers V0 through Vx into
 * memory, starting at the address in I.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_I_VX(chip8_t *ch8, chip8_op_t op)
{
    for (uint8_t i = 0; i <= op.x; ++i)
        ch8->ram[ch8->I++] = ch8->V[i];

    return CHIP8_OK;
}

/**
 * @brief Read registers V0 through Vx from memory starting at location I
 * [FX65]
 *
 * @details The interpreter reads values from memory starting at location I
 * into registers V0 through Vx.
 *
 * @param ch8 Chip-8 instance
 * @param op  Parsed opcode
 * @return    Error code
 */
chip8_err_t __LD_VX_I(chip8_t *ch8, chip8_op_t op)
{
    for (uint8_t i = 0; i <= op.x; ++i)
        ch8->V[i] = ch8->ram[ch8->I++];

    return CHIP8_OK;
}

/* -------------------------------------------------------------------------- */
