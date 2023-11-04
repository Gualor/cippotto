#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "chip8.h"

void decode(uint16_t opcode)
{
    uint16_t _NNN = (opcode & 0x0FFF);
    uint16_t __NN = (opcode & 0x00FF);
    uint16_t ___N = (opcode & 0x000F);
    uint16_t X = _NNN >> 8;
    uint16_t Y = __NN >> 4;

    if (opcode == 0x00E0)
        printf("clear\n");
    else if (opcode == 0x00EE)
        printf("Return\n");
    else if (opcode & 0x1000)
        printf("jump 0x%04X\n", _NNN);
    else if (opcode & 0x2000)
        printf("call 0x%04X\n", _NNN);
    else if (opcode & 0x3000)
        printf("if (R%X != %d) then\n", X, __NN);
    else if (opcode & 0x4000)
        printf("if (R%X == %d) then\n", X, __NN);
    else if (opcode & 0x5000)
        printf("if (R%X != R%X) then\n", X, Y);
    else if (opcode & 0x6000)
        printf("R%X := %d\n", X, __NN);
    else if (opcode & 0x7000)
        printf("R%X += %d\n", X, __NN);
    else if (opcode & 0x8000)
    {
        if (___N == 0x0)
            printf("R%X := R%X\n", X, Y);
        else if (___N == 0x1)
            printf("R%X |= R%X\n", X, Y);
        else if (___N == 0x2)
            printf("R%X &= R%X\n", X, Y);
        else if (___N == 0x3)
            printf("R%X ^= R%X\n", X, Y);
        else if (___N == 0x4)
            printf("R%X += R%X\t[RF := 1 on carry]\n", X, Y);
        else if (___N == 0x5)
            printf("R%X -= R%X\t[RF := 0 on borrow]\n", X, Y);
        else if (___N == 0x6)
            printf("R%X >>= R%X\t[RF := old least significant bit]\n", X, Y);
        else if (___N == 0x7)
            printf("R%X =- R%X\t[RF := 0 on borrow]\n", X, Y);
        else if (___N == 0xE)
            printf("R%X <<= R%X\t[RF := old most significant bit]\n", X, Y);
    }
    else if (opcode & 0x9000)
        printf("if (R%X == R%X) then\n", X, Y);
    else if (opcode & 0xA000)
        printf("i := %d\n", _NNN);
    else if (opcode & 0xB000)
        printf("jump 0x%04X + v0\n", _NNN);
    else if (opcode & 0xC000)
        printf("R%X := random[0-255] & %d\n", X, __NN);
    else if (opcode & 0xD000)
        printf("sprite R%X R%X %d\t[RF := 1 on collision]\n", X, Y, ___N);
    else if (opcode & 0xE000)
    {
        if (__NN == 0x9E)
            printf("if (R%X -key) then\n", X);
        if (__NN == 0xA1)
            printf("if (R%X key) then\n", X);
    }
    else if (opcode & 0xF000)
    {
        if (__NN == 0x07)
            printf("R%X := delay\n", X);
        if (__NN == 0x0A)
            printf("R%X := key\n", X);
        if (__NN == 0x15)
            printf("delay := R%X\n", X);
        if (__NN == 0x18)
            printf("buzzer := R%X\n", X);
        if (__NN == 0x1E)
            printf("I += R%X\n", X);
        if (__NN == 0x29)
            printf("I := hex R%X\n", X);
        if (__NN == 0x33)
            printf("bcd R%X\n", X);
        if (__NN == 0x55)
            printf("save R%X\n", X);
        if (__NN == 0x65)
            printf("load R%X\n", X);
    }
    else
        printf("Unknown: %04X\n", opcode);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s [file]\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    long bytes = ftell(fp);
    rewind(fp);

    for (long i = 0; i < bytes; i += 2)
    {
        uint16_t opcode = ((uint16_t)fgetc(fp) << 8) | fgetc(fp);
        printf("%ld\t", i);
        decode(opcode);
    }

    fclose(fp);

    return 0;
}
