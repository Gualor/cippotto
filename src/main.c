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

    uint16_t opcode;
    chip8_cmd_t chip8_cmd;
    chip8_err_t err = CHIP8_OK;
    for (;;)
    {
        err = chip8_fetch(&opcode);
        if (err)
            break;

        err = chip8_decode(&chip8_cmd, opcode);
        if (err)
            break;

        err = chip8_cmd();
        if (err)
            break;
    }

    return (err == CHIP8_EXIT);
}
