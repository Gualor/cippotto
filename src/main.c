/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "style_cyber.h"

#include "constants.h"
#include "chip8.h"

/* Function Prototypes ------------------------------------------------------ */

static void InitCippottoGui(void);
static void UpdateCippottoGui(chip8_t *ch8);
static void ReadInputKeys(chip8_t *ch8);
static void UpdateRegsView(chip8_t *ch8);
static void UpdateRegValue(char *text, void *reg, int size, int *value,
                           bool *edit, Rectangle rec);
static void UpdateGameView(chip8_t *ch8);
static void UpdateASMView(chip8_t *ch8);

/* Global variables --------------------------------------------------------- */

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

// ScrollPanel: Assembly
static char asm_buffer[GUI_ASM_BUFFER_LEN][CHIP8_DECODE_STR_SIZE] = {0};
static int asm_counter = 0;
static int asm_head = 0;
static Rectangle asm_content = {0};
static Rectangle asm_view = {0};
static Vector2 asm_scroll = {0, 0};

// Style colors
static Color background_color = {0};
static Color line_color = {0};
static Color border_color[4] = {0};
static Color base_color[4] = {0};
static Color text_color[4] = {0};

// CLI arguments
static char *rom_path = NULL;
static int rom_addr = CHIP8_RAM_PROGRAM;

/* Function definitions ----------------------------------------------------- */

void UpdateRegsView(chip8_t *ch8)
{
    GuiGroupBox(gui_layout[LAYOUT_REGS], "Registers");
    for (uint8_t i = 0; i < CHIP8_REGS_NUM; ++i)
    {
        char Vi[5];
        sprintf(Vi, "V%X ", i);
        UpdateRegValue(Vi, &ch8->V[i], sizeof(uint8_t), &Vi_value[i],
                       &Vi_edit[i], gui_layout[LAYOUT_V0 + i]);
    }
    GuiLine(gui_layout[LAYOUT_TIM], NULL);
    UpdateRegValue("DT ", &ch8->DT, sizeof(uint8_t), &DT_value, &DT_edit,
                   gui_layout[LAYOUT_DT]);
    UpdateRegValue("ST ", &ch8->ST, sizeof(uint8_t), &ST_value, &ST_edit,
                   gui_layout[LAYOUT_ST]);
    GuiLine(gui_layout[LAYOUT_SPE], NULL);
    UpdateRegValue("I ", &ch8->I, sizeof(uint16_t), &I_value, &I_edit,
                   gui_layout[LAYOUT_I]);
    UpdateRegValue("PC ", &ch8->PC, sizeof(uint16_t), &PC_value, &PC_edit,
                   gui_layout[LAYOUT_PC]);
    UpdateRegValue("SP ", &ch8->SP, sizeof(uint16_t), &SP_value, &SP_edit,
                   gui_layout[LAYOUT_SP]);
}

void UpdateRegValue(char *text, void *reg, int size, int *value, bool *edit,
                    Rectangle rec)
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

void UpdateGameView(chip8_t *ch8)
{
    GuiGroupBox(gui_layout[LAYOUT_GAME], GUI_GAME_TITLE);

    Rectangle panel = gui_layout[LAYOUT_DRAW];
    float scale_x = panel.width / CHIP8_DISPLAY_WIDTH;
    float scale_y = panel.height / CHIP8_DISPLAY_HEIGHT;

    for (uint8_t x = 0; x < CHIP8_DISPLAY_WIDTH; ++x)
    {
        for (uint8_t y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y)
        {
            Rectangle pixel = {
                panel.x + (x * scale_x),
                panel.y + (y * scale_y),
                scale_x,
                scale_y,
            };

            if (ch8->display[x + y * CHIP8_DISPLAY_WIDTH])
                DrawRectangleRec(pixel, line_color);
        }
    }
}

void UpdateASMView(chip8_t *ch8)
{
    Rectangle panel = gui_layout[LAYOUT_ASM];

    GuiScrollPanel(panel, NULL, asm_content, &asm_scroll, &asm_view);
    BeginScissorMode(asm_view.x, asm_view.y, asm_view.width, asm_view.height);

    uint16_t raw = ((uint16_t)ch8->ram[ch8->PC] << 8) | ch8->ram[ch8->PC + 1];
    chip8_op_t op;
    chip8_cmd_t cmd;
    char cmd_str[CHIP8_DECODE_STR_SIZE];

    chip8_parse(&op, raw);
    chip8_decode(&cmd, cmd_str, op);

    asm_head = (asm_head + 1) % GUI_ASM_BUFFER_LEN;
    sprintf(asm_buffer[asm_head], "0x%04X\t%s", ch8->PC, cmd_str);
    if (asm_counter < GUI_ASM_BUFFER_LEN)
    {
        ++asm_counter;
        if (asm_counter > GUI_ASM_TEXT_LINES)
        {
            asm_content.height += GUI_GRID_SPACING;
            asm_scroll.y -= GUI_GRID_SPACING;
        }
    }

    float posx = panel.x + asm_scroll.x + GUI_GRID_SPACING;
    float posy = panel.y + asm_scroll.y + asm_content.height - GUI_GRID_SPACING;
    int idx = asm_head;
    for (int i = 0; i < asm_counter; ++i)
    {
        DrawText(
            asm_buffer[idx--],
            posx,
            posy - (GUI_GRID_SPACING * i),
            GUI_ASM_FONT_SIZE,
            (i == 0) ? text_color[2] : text_color[0]
        );

        if (idx < 0) idx += GUI_ASM_BUFFER_LEN;
    }

    EndScissorMode();
    DrawRectangleRec((Rectangle){panel.x, panel.y, panel.width - 14, 6},
                     background_color);
    GuiGroupBox(panel, GUI_ASM_TITLE);
}

void ReadInputKeys(chip8_t *ch8)
{
    memset(ch8->keys, 0x0, sizeof(uint8_t) * CHIP8_KEYS_SIZE);
    for (uint8_t i = 0; i < CHIP8_KEYS_SIZE; ++i)
        ch8->keys[i] = IsKeyDown(gui_keys[i]);
}

void InitCippottoGui(void)
{
    InitWindow(GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, GUI_WINDOW_TITLE);
    SetTargetFPS(CHIP8_CLOCK_HZ);
    GuiLoadStyleCyber();

    background_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    line_color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));

    border_color[0] = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
    border_color[1] = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED));
    border_color[2] = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED));
    border_color[3] = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED));

    base_color[0] = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
    base_color[1] = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
    base_color[2] = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
    base_color[3] = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_DISABLED));

    text_color[0] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
    text_color[1] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED));
    text_color[2] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED));
    text_color[3] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_DISABLED));

    asm_content.x = 0;
    asm_content.y = 0;
    asm_content.width = gui_layout[LAYOUT_ASM].width - 15;
    asm_content.height = gui_layout[LAYOUT_ASM].height;
}

void UpdateCippottoGui(chip8_t *ch8)
{
    BeginDrawing();
    ClearBackground(background_color);
    UpdateRegsView(ch8);
    UpdateGameView(ch8);
    UpdateASMView(ch8);
    EndDrawing();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <rom> <addr, optional>\n", argv[0]);
        exit(1);
    }
    rom_path = argv[1];
    if (argc > 2) rom_addr = atoi(argv[2]);

    FILE *rom = fopen(rom_path, "rb");
    if (rom == NULL)
    {
        fprintf(stderr, "Error opening the ROM file: %s\n", rom_path);
        exit(1);
    }

    chip8_t chip8;
    chip8_cfg_t chip8_cfg = {
        .rom = rom,
        .addr = rom_addr,
    };
    chip8_init(&chip8, chip8_cfg);
    fclose(rom);

    InitCippottoGui();
    while (!WindowShouldClose() && !chip8_run(&chip8))
    {
        ReadInputKeys(&chip8);
        UpdateCippottoGui(&chip8);
    }
    CloseWindow();

    return 0;
}

/* -------------------------------------------------------------------------- */
