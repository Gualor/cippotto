#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chip8.h"

// General purpose registers
#define REGISTER_NUM 16
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
#define STACK_SIZE 16
static uint16_t stack[STACK_SIZE];

// RAM memory map
#define RAM_ADDR_START 0x000
#define RAM_ADDR_PROG 0x200
#define RAM_ADDR_END 0xFFF
#define RAM_SIZE 4096
static uint8_t ram[RAM_SIZE];
static uint16_t prog_bytes;

/**
 * @brief CHIP8 interpreter initialization
 *
 * @param rom ROM path
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_init(char *rom)
{
    srand(time(NULL));

    FILE *file = fopen(rom, "rb");
    if (file == NULL)
    {
        perror("Error opening the file");
        return CHIP8_ROM_LOAD_ERROR;
    }

    fseek(file, 0, SEEK_END);
    prog_bytes = ftell(file);
    rewind(file);

    fread(&ram[RAM_ADDR_PROG], sizeof(uint8_t), prog_bytes, file);
    fclose(file);

    memset(V, 0x0, sizeof(uint8_t) * REGISTER_NUM);
    memset(stack, 0x0, sizeof(uint16_t) * STACK_SIZE);

    I = 0;
    DT = 0;
    ST = 0;
    PC = RAM_ADDR_PROG;
    SP = 0;

    return CHIP8_OK;
}

/**
 * @brief CHIP8 fetch new instruction
 *
 * @param opcode Destination 16-bit opcode
 * @return chip8_err_t Error code
 */
chip8_err_t chip8_fetch(uint16_t *opcode)
{
    *opcode = ((uint16_t)ram[PC] << 8) | ram[PC + 1];
    PC += sizeof(uint16_t);
    return CHIP8_OK;
}

chip8_err_t chip8_execute(uint16_t opcode)
{
    chip8_err_t err = CHIP8_OK;

    uint16_t O___ = (opcode & 0xF000);
    uint16_t _NNN = (opcode & 0x0FFF);
    uint16_t __NN = (opcode & 0x00FF);
    uint16_t ___N = (opcode & 0x000F);
    uint16_t X = _NNN >> 8;
    uint16_t Y = __NN >> 4;

    uint16_t addr = PC - sizeof(uint16_t);
    printf("0x%04X\t0x%04X\t", addr, opcode);

    switch (O___)
    {
    case 0x0000:
        switch (__NN)
        {
        case 0xE0:
            printf("CLS\n");
            break;

        case 0xEE:
            printf("RET\n");
            if (SP > 0)
                PC = stack[SP--];
            else
                err = CHIP8_EXIT;
            break;

        default:
            printf("SYS 0x%04X\t[ignored]\n", _NNN);
            break;
        }
        break;

    case 0x1000:
        printf("JP 0x%04X\n", _NNN);
        if (_NNN <= RAM_ADDR_END)
            PC = _NNN;
        else
            err = CHIP8_INVALID_ADDR;
        break;

    case 0x2000:
        printf("CALL 0x%04X\n", _NNN);
        if (SP < STACK_SIZE - 1)
        {
            stack[++SP] = PC;
            PC = _NNN;
        }
        else
            err = CHIP8_STACK_OVERFLOW;
        break;

    case 0x3000:
        printf("SE V%X, %d\n", X, __NN);
        if (V[X] == __NN)
            PC += sizeof(uint16_t);
        break;

    case 0x4000:
        printf("SNE V%X, %d\n", X, __NN);
        if (V[X] != __NN)
            PC += sizeof(uint16_t);
        break;

    case 0x5000:
        printf("SE V%X, V%X\n", X, Y);
        if (V[X] == V[Y])
            PC += sizeof(uint16_t);
        break;

    case 0x6000:
        printf("LD V%X, %d\n", X, __NN);
        V[X] = __NN;
        break;

    case 0x7000:
        printf("ADD V%X, %d\n", X, __NN);
        V[X] += __NN;
        break;

    case 0x8000:
        switch (___N)
        {
        case 0x0:
            printf("LD V%X, V%X\n", X, Y);
            V[X] = V[Y];
            break;

        case 0x1:
            printf("OR V%X, V%X\n", X, Y);
            V[X] |= V[Y];
            break;

        case 0x2:
            printf("AND V%X, V%X\n", X, Y);
            V[X] &= V[Y];
            break;

        case 0x3:
            printf("XOR V%X, V%X\n", X, Y);
            V[X] ^= V[Y];
            break;

        case 0x4:
            printf("ADD, V%X, V%X\t[Set VF = carry]\n", X, Y);
            uint16_t res = V[X] + V[Y];
            V[0xF] = res >> 8;
            V[X] = (uint8_t)res;
            break;

        case 0x5:
            printf("SUB V%X, V%X\t[Set VF = NOT borrow]\n", X, Y);
            V[0xF] = (V[X] > V[Y]) ? 1 : 0;
            V[X] -= V[Y];
            break;

        case 0x6:
            printf("SHR V%X, V%X\t[set VF = LSB]\n", X, Y);
            V[0xF] = (V[X] & 0x1) ? 1 : 0;
            V[X] >>= V[Y];
            break;

        case 0x7:
            printf("SUBN V%X, V%X\t[set VF = NOT borrow]\n", X, Y);
            V[0xF] = (V[Y] > V[X]) ? 1 : 0;
            V[X] = V[Y] - V[X];
            break;

        case 0xE:
            printf("SHL V%X, V%X\t[set VF = MSB]\n", X, Y);
            V[0xF] = (V[X] & 0x64) ? 1 : 0;
            V[X] <<= V[Y];
            break;

        default:
            err = CHIP8_UNKNOWN_CMD;
            break;
        }
        break;

    case 0x9000:
        printf("SNE V%X, V%X\n", X, Y);
        if (V[X] != V[Y])
            PC += sizeof(uint16_t);
        break;

    case 0xA000:
        printf("LD I, %d\n", _NNN);
        I = _NNN;
        break;

    case 0xB000:
        printf("JP V0, 0x%04X\n", _NNN);
        PC = V[0] + _NNN;
        break;

    case 0xC000:
        printf("RND V%X, %d\n", X, __NN);
        uint8_t rnd = rand() % 256;
        V[X] = rnd & __NN;
        break;

    case 0xD000:
        printf("DRW V%X, V%X, %d\t[set VF = collision]\n", X, Y, ___N);
        /**
         * Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
         *
         * The interpreter reads n bytes from memory, starting at the address
         * stored in I. These bytes are then displayed as sprites on screen at
         * coordinates (Vx, Vy). Sprites are XORed onto the existing screen
         */
        break;

    case 0xE000:
        switch (__NN)
        {
        case 0x9E:
            printf("SKP V%X\n", X);
            /**
             * TODO: Skip next instruction if key with the value of Vx is pressed
             *
             * Checks the keyboard, and if the key corresponding to the value
             * of Vx is currently in the down position, PC is increased by 2
             */
            break;

        case 0xA1:
            printf("SKNP V%X\n", X);
            /**
             * TODO: Skip next instruction if key with the value of Vx is not pressed
             *
             * Checks the keyboard, and if the key corresponding to the value
             * of Vx is currently in the up position, PC is increased by 2
             */
            break;

        default:
            err = CHIP8_UNKNOWN_CMD;
            break;
        }
        break;

    case 0xF000:
        switch (__NN)
        {
        case 0x07:
            printf("LD V%X, DT\n", X);
            V[X] = DT;
            break;

        case 0x0A:
            printf("LD V%X, K\n", X);
            /**
             * TODO: Wait for a key press, store the value of the key in Vx
             *
             * All execution stops until a key is pressed, then the value of
             * that key is stored in Vx
             */
            break;

        case 0x15:
            printf("LD DT, V%X\n", X);
            DT = V[X];
            break;

        case 0x18:
            printf("LD ST, V%X\n", X);
            ST = V[X];
            break;

        case 0x1E:
            printf("ADD I, V%X\n", X);
            I += V[X];
            break;

        case 0x29:
            printf("LD F, V%X\n", X);
            /**
             * TODO: Set I = location of sprite for digit Vx
             *
             * The value of I is set to the location for the hexadecimal sprite
             * corresponding to the value of Vx
             */
            break;

        case 0x33:
            printf("LD B, V%X\n", X);
            /**
             * TODO: Store BCD representation of Vx in memory locations I, I+1, and I+2
             *
             * The interpreter takes the decimal value of Vx, and places the
             * hundreds digit in memory at location in I, the tens digit at
             * location I+1, and the ones digit at location I+2
             */
            break;

        case 0x55:
            printf("LD [I], V%X\n", X);
            /**
             * TODO: Store registers V0 through Vx in memory starting at location I
             *
             * The interpreter copies the values of registers V0 through Vx
             * into memory, starting at the address in I
             */
            break;

        case 0x65:
            printf("LD V%X, [I]\n", X);
            /**
             * TODO: Read registers V0 through Vx from memory starting at location I
             *
             * The interpreter reads values from memory starting at location I
             * into registers V0 through Vx
             */
            break;

        default:
            err = CHIP8_UNKNOWN_CMD;
            break;
        }
        break;

    default:
        err = CHIP8_UNKNOWN_CMD;
        break;
    }

    return err;
}
