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

void draw_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    DrawRectangle(
        x * DISPLAY_ZOOM,
        y * DISPLAY_ZOOM,
        DISPLAY_ZOOM,
        DISPLAY_ZOOM,
        (color) ? WHITE : BLACK);
}

void draw_display(void)
{
    for (int x = 0; x < DISPLAY_WIDTH; ++x)
    {
        for (int y = 0; y < DISPLAY_HEIGHT; ++y)
        {
            draw_pixel(x, y, chip8_display[x][y]);
        }
    }
}

void get_input(void)
{
    static const int raylib_keys[] = {
        KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX,
        KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F
    };

    memset(chip8_keys, 0x0, sizeof(chip8_keys));

    for (int i = 0; i < KEYS_NUM; ++i)
        chip8_keys[i] = IsKeyDown(raylib_keys[i]);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <rom>\n", argv[0]);
        exit(1);
    }
    char *rom_path = argv[1];

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CHIP8 Emulator");
    SetTargetFPS(60);

    chip8_init(rom_path, CHIP8_RAM_PROGRAM);

    uint16_t opcode;
    chip8_cmd_t chip8_cmd;
    chip8_err_t err = CHIP8_OK;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        get_input();

        err = chip8_fetch(&opcode);
        if (err)
            break;

        err = chip8_decode(&chip8_cmd, opcode);
        if (err)
            break;

        err = chip8_cmd();
        if (err)
            break;

        err = chip8_tick();
        if (err)
            break;

        draw_display();

        EndDrawing();
    }

    return (err == CHIP8_EXIT);
}

/* -------------------------------------------------------------------------- */
