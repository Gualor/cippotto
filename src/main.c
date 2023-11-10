/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <raylib.h>

#include "chip8.h"

/* Definitions -------------------------------------------------------------- */

#define WINDOW_ZOOM 10
#define WINDOW_WIDTH (CHIP8_DISPLAY_WIDTH * WINDOW_ZOOM)
#define WINDOW_HEIGHT (CHIP8_DISPLAY_HEIGHT * WINDOW_ZOOM)
#define TARGET_FPS 120

/* Function definitions ----------------------------------------------------- */

void platform_draw_screen(uint8_t *display)
{
    BeginDrawing();

    for (uint8_t x = 0; x < CHIP8_DISPLAY_WIDTH; ++x)
    {
        for (uint8_t y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y)
        {
            DrawRectangle(
                x * WINDOW_ZOOM,
                y * WINDOW_ZOOM,
                WINDOW_ZOOM,
                WINDOW_ZOOM,
                (display[x + y * CHIP8_DISPLAY_WIDTH]) ? WHITE : BLACK);
        }
    }

    EndDrawing();
}

void platform_read_keys(uint8_t *keys)
{
    static const int raylib_keys[] = {
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, // 1, 2, 3, C
        KEY_Q,   KEY_W,   KEY_E,     KEY_R,    // 4, 5, 6, D
        KEY_A,   KEY_S,   KEY_D,     KEY_F,    // 7, 8, 9, E
        KEY_Z,   KEY_X,   KEY_C,     KEY_V,    // A, 0, B, F
    };

    memset(keys, 0x0, sizeof(uint8_t) * CHIP8_KEYS_SIZE);

    for (uint8_t i = 0; i < CHIP8_KEYS_SIZE; ++i)
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
        .addr = CHIP8_RAM_PROGRAM};

    chip8_t chip8;
    chip8_init(&chip8, &chip8_cfg);
    fclose(rom);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chip8 Emulator");
    SetTargetFPS(TARGET_FPS);

    chip8_err_t err = CHIP8_OK;
    while (!WindowShouldClose() && !err)
    {
        platform_read_keys(chip8.keys);
        err = chip8_run(&chip8);
        platform_draw_screen(chip8.display);
    }

    return (err == CHIP8_EXIT);
}

/* -------------------------------------------------------------------------- */
