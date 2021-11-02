#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * Dump CHIP-8 ROM binary in hex format.
 */
void chip8_dump(FILE *rom_ptr)
{
    size_t ret = 1;
    uint16_t inbyte[8];
    uint16_t addr_count = 0;

    while (1)
    {
        ret = fread(inbyte, 2, 8, rom_ptr);
        if (ret==0)
            break;

        printf("%08x ", addr_count);
        for (int i=0; i<ret; i++)
        {
            printf("%04x ", inbyte[i]);
        }
        printf("\n");

        addr_count += 16;
    }
}

/**
 * Main function.
 */
int main(int argc, char **argv)
{
    // Read ROM file
    char *rom_path = argv[1];
    FILE *fptr = fopen(rom_path, "r");
    if(fptr == NULL)
    {
        printf("%s\tCHIP-8 ROM path does not exist.\n", rom_path);   
        exit(1);             
    }

    // Dump ROM in hex format
    chip8_dump(fptr);

    return 0;
}