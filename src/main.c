/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "style_cyber.h"
#include "chip8.h"

/* Definitions -------------------------------------------------------------- */

#define WINDOW_WIDTH  1520
#define WINDOW_HEIGHT 552

#define LAYOUT_GROUP_REGISTERS 0
#define LAYOUT_VALUE_V0        1
#define LAYOUT_LINE_TIMERS     17
#define LAYOUT_VALUE_DT        18
#define LAYOUT_VALUE_ST        19
#define LAYOUT_LINE_SPECIAL    20
#define LAYOUT_VALUE_I         21
#define LAYOUT_VALUE_PC        22
#define LAYOUT_VALUE_SP        23
#define LAYOUT_GROUP_GAME      24
#define LAYOUT_GROUP_ASM       25

/* Function Prototypes ------------------------------------------------------ */

static void UpdateRegValue(char *text, void *reg, int size, int *value, bool *edit, Rectangle rec);
static void UpdateRegsView(chip8_t *ch8);
static void UpdateGameView(chip8_t *ch8);
static void UpdateASMView(chip8_t *ch8);
static void ReadInputKeys(chip8_t *ch8);

/* Global variables --------------------------------------------------------- */

static const int raylib_keys[] = {
    KEY_ZERO,
    KEY_ONE,
    KEY_TWO,
    KEY_THREE,
    KEY_FOUR,
    KEY_FIVE,
    KEY_SIX,
    KEY_SEVEN,
    KEY_EIGHT,
    KEY_NINE,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
};

static const Rectangle layout[] = {
    (Rectangle){   24,  24, 264, 504 }, // GroupBox: Registers
    (Rectangle){   72,  48,  72,  24 }, // ValueBox: V0
    (Rectangle){   72,  80,  72,  24 }, // ValueBox: V1
    (Rectangle){   72, 112,  72,  24 }, // ValueBox: V2
    (Rectangle){   72, 144,  72,  24 }, // ValueBox: V3
    (Rectangle){   72, 176,  72,  24 }, // ValueBox: V4
    (Rectangle){   72, 208,  72,  24 }, // ValueBox: V5
    (Rectangle){   72, 240,  72,  24 }, // ValueBox: V6
    (Rectangle){   72, 272,  72,  24 }, // ValueBox: V7
    (Rectangle){  192,  48,  72,  24 }, // ValueBox: V8
    (Rectangle){  192,  80,  72,  24 }, // ValueBox: V9
    (Rectangle){  192, 112,  72,  24 }, // ValueBox: VA
    (Rectangle){  192, 144,  72,  24 }, // ValueBox: VB
    (Rectangle){  192, 176,  72,  24 }, // ValueBox: VC
    (Rectangle){  192, 208,  72,  24 }, // ValueBox: VD
    (Rectangle){  192, 240,  72,  24 }, // ValueBox: VE
    (Rectangle){  192, 272,  72,  24 }, // ValueBox: VF
    (Rectangle){   48, 312, 216,  16 }, // Line:     Timers
    (Rectangle){   72, 344,  72,  24 }, // ValueBox: DT
    (Rectangle){  192, 344,  72,  24 }, // ValueBox: ST
    (Rectangle){   48, 384, 216,  16 }, // Line:     Special
    (Rectangle){   72, 416,  72,  24 }, // ValueBox: I
    (Rectangle){   72, 448,  72,  24 }, // ValueBox: PC
    (Rectangle){   72, 480,  72,  24 }, // ValueBox: SP
    (Rectangle){  312,  24, 896, 504 }, // GroupBox: Game
    (Rectangle){ 1232,  24, 264, 504 }, // GroupBox: Assembly
};

static Color background_color = BLACK;
static Color foreground_color = WHITE;

// ValueBox: V0-VF
static int Vi_value[CHIP8_REGS_NUM] = {0};
static bool Vi_edit[CHIP8_REGS_NUM] = {false};

// ValueBox: DT
static int DT_value = 0;
static bool DT_edit = false;

// ValueBox: ST
static int ST_value = 0;
static bool ST_edit = false;

// ValueBox: I
static int I_value = 0;
static bool I_edit = false;

// ValueBox: PC
static int PC_value = 0;
static bool PC_edit = false;

// ValueBox: SP
static int SP_value = 0;
static bool SP_edit = false;

/* Function definitions ----------------------------------------------------- */

void UpdateRegValue(char *text, void *reg, int size, int *value, bool *edit, Rectangle rec)
{
    if (!(*edit))
    {
        *value = 0;
        memcpy(value, reg, size);
    }

    if (GuiValueBox(rec, text, value, 0x00, (size == 1) ? 0xFF : 0xFFFF, *edit))
    {
        if (*edit) memcpy(reg, value, size);
        *edit = !(*edit);
    }
}

void UpdateRegsView(chip8_t *ch8)
{
    GuiGroupBox(layout[LAYOUT_GROUP_REGISTERS], "Registers");
    for (uint8_t i = 0; i < CHIP8_REGS_NUM; ++i)
    {
        char Vi[5];
        sprintf(Vi, "V%X ", i);
        UpdateRegValue(Vi, &ch8->V[i], sizeof(uint8_t), &Vi_value[i], &Vi_edit[i], layout[LAYOUT_VALUE_V0 + i]);
    }
    GuiLine(layout[LAYOUT_LINE_TIMERS], NULL);
    UpdateRegValue("DT ", &ch8->DT, sizeof(uint8_t), &DT_value, &DT_edit, layout[LAYOUT_VALUE_DT]);
    UpdateRegValue("ST ", &ch8->ST, sizeof(uint8_t), &ST_value, &ST_edit, layout[LAYOUT_VALUE_ST]);
    GuiLine(layout[LAYOUT_LINE_SPECIAL], NULL);
    UpdateRegValue("I ", &ch8->I, sizeof(uint16_t), &I_value, &I_edit, layout[LAYOUT_VALUE_I]);
    UpdateRegValue("PC ", &ch8->PC, sizeof(uint16_t), &PC_value, &PC_edit, layout[LAYOUT_VALUE_PC]);
    UpdateRegValue("SP ", &ch8->SP, sizeof(uint16_t), &SP_value, &SP_edit, layout[LAYOUT_VALUE_SP]);
}

void UpdateGameView(chip8_t *ch8)
{
    Rectangle rect = layout[LAYOUT_GROUP_GAME];
    float scale_x = rect.width / CHIP8_DISPLAY_WIDTH;
    float scale_y = rect.height / CHIP8_DISPLAY_HEIGHT;

    for (uint8_t x = 0; x < CHIP8_DISPLAY_WIDTH; ++x)
    {
        for (uint8_t y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y)
        {
            Rectangle pixel = {
                rect.x + (x * scale_x),
                rect.y + (y * scale_y),
                scale_x,
                scale_y,
            };

            if (ch8->display[x + y * CHIP8_DISPLAY_WIDTH])
                DrawRectangleRec(pixel, foreground_color);
        }
    }

    GuiGroupBox(rect, "Game");
}

void UpdateASMView(chip8_t *ch8)
{
    GuiGroupBox(layout[LAYOUT_GROUP_ASM], "Assembly");
}

void ReadInputKeys(chip8_t *ch8)
{
    memset(ch8->keys, 0x0, sizeof(uint8_t) * CHIP8_KEYS_SIZE);

    for (uint8_t i = 0; i < CHIP8_KEYS_SIZE; ++i)
        ch8->keys[i] = IsKeyDown(raylib_keys[i]);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stdout, "Usage: %s <rom>\n", argv[0]);
        exit(1);
    }

    FILE *rom = fopen(argv[1], "rb");
    if (rom == NULL)
    {
        fprintf(stderr, "Error opening the ROM file: %s\n", argv[1]);
        exit(1);
    }

    chip8_t chip8;
    chip8_cfg_t chip8_cfg = {
        .rom = rom,
        .addr = CHIP8_RAM_PROGRAM,
    };
    chip8_init(&chip8, chip8_cfg);
    fclose(rom);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cippotto GUI");
    SetTargetFPS(CHIP8_CLOCK_HZ);

    GuiLoadStyleCyber();
    background_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    foreground_color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));

    while (!WindowShouldClose() && !chip8_run(&chip8))
    {
        BeginDrawing();
        ClearBackground(background_color);
        ReadInputKeys(&chip8);
        UpdateRegsView(&chip8);
        UpdateGameView(&chip8);
        UpdateASMView(&chip8);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

/* -------------------------------------------------------------------------- */
