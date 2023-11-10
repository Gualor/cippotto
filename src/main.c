/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <raylib.h>

#include "chip8.h"

/* Definitions -------------------------------------------------------------- */

#define DISPLAY_ZOOM 10
#define WINDOW_WIDTH (DISPLAY_WIDTH * DISPLAY_ZOOM)
#define WINDOW_HEIGHT (DISPLAY_HEIGHT * DISPLAY_ZOOM)

/* Function definitions ----------------------------------------------------- */

void draw_display(uint8_t *display)
{
    for (uint8_t x = 0; x < DISPLAY_WIDTH; ++x)
    {
        for (uint8_t y = 0; y < DISPLAY_HEIGHT; ++y)
        {
            DrawRectangle(
                x * DISPLAY_ZOOM,
                y * DISPLAY_ZOOM,
                DISPLAY_ZOOM,
                DISPLAY_ZOOM,
                (display[x + y * DISPLAY_WIDTH]) ? WHITE : BLACK
            );
        }
    }
}

void get_input(uint8_t *keys)
{
    // Remap keys to mimic Chip8 keypad layout
    static const int raylib_keys[] = {
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, // 1, 2, 3, C
        KEY_Q,   KEY_W,   KEY_E,     KEY_R,    // 4, 5, 6, D
        KEY_A,   KEY_S,   KEY_D,     KEY_F,    // 7, 8, 9, E
        KEY_Z,   KEY_X,   KEY_C,     KEY_V,    // A, 0, B, F
    };

    memset(keys, 0x0, sizeof(uint8_t) * KEYS_SIZE);

    for (uint8_t i = 0; i < KEYS_SIZE; ++i)
        keys[i] = IsKeyDown(raylib_keys[i]);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stdout, "Usage: %s <rom>\n", argv[0]);
        exit(0);
    }

    FILE *rom = fopen(argv[1], "rb");
    if (rom == NULL)
    {
        fprintf(stderr, "Error opening the ROM file: %s\n", argv[1]);
        exit(1);
    }

    chip8_cfg_t chip8_cfg = {
        .rom = rom,
        .addr = CHIP8_RAM_PROGRAM
    };

    chip8_t chip8;
    chip8_init(&chip8, &chip8_cfg);
    fclose(rom);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chip8 Emulator");
    SetTargetFPS(120);

    chip8_err_t err = CHIP8_OK;
    while (!WindowShouldClose() && !err)
    {
        BeginDrawing();

        get_input(chip8.keys);
        err = chip8_run(&chip8);
        draw_display(chip8.display);

        EndDrawing();
    }

    return (err == CHIP8_EXIT);
}

/* -------------------------------------------------------------------------- */
