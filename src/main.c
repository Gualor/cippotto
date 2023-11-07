#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "chip8.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <rom>\n", argv[0]);
        exit(1);
    }

    chip8_init(argv[1]);

    chip8_err_t err = CHIP8_OK;
    while (err == CHIP8_OK)
    {
        uint16_t opcode;
        err = chip8_fetch(&opcode);
        err |= chip8_execute(opcode);
    }

    return (err == CHIP8_EXIT);
}
