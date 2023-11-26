/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "style_cyber.h"
#include "constants.h"
#include "chip8.h"

/* Function Prototypes ------------------------------------------------------ */

static void InitCippottoGui(void);
static void UpdateCippottoGui(void);
static void ReadInputKeys(void);
static void UpdateRegsView(void);
static void UpdateRegValue(char *text, void *reg, int size, int *value,
                           bool *edit, Rectangle bounds);
static void UpdateGameView(void);
static void UpdateFlowView(void);
static void UpdateASMView(void);
static void ResetASMView(void);

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

// Button: Play
static bool play_state = false;

// Button: Step
static bool step_state = false;

// Button: Restart
static bool rst_state = false;

// ScrollPanel: Assembly
static char asm_buffer[GUI_ASM_BUFFER_LEN][CHIP8_DECODE_STR_SIZE] = {0};
static int asm_counter = 0;
static int asm_head = 0;
static int asm_opcode = 0;
static Rectangle asm_content = {0};
static Rectangle asm_view = {0};
static Vector2 asm_scroll = {0, 0};

// Style parameters
static int text_size = GUI_GRID_SPACING;
static Color background_color = {0};
static Color line_color = {0};
static Color border_color[4] = {0};
static Color base_color[4] = {0};
static Color text_color[4] = {0};

// Chip-8 instance
static chip8_t chip8 = {0};

/* Function definitions ----------------------------------------------------- */

/**
 * @brief Update registers view
 * 
 */
void UpdateRegsView(void)
{
    GuiGroupBox(gui_layout[LAYOUT_REGS], "Registers");
    for (uint8_t i = 0; i < CHIP8_REGS_NUM; ++i)
    {
        char Vi[5];
        sprintf(Vi, "V%X ", i);
        UpdateRegValue(Vi, &chip8.V[i], sizeof(uint8_t), &Vi_value[i],
                       &Vi_edit[i], gui_layout[LAYOUT_V0 + i]);
    }
    GuiLine(gui_layout[LAYOUT_TIM], NULL);
    UpdateRegValue("DT ", &chip8.DT, sizeof(uint8_t), &DT_value, &DT_edit,
                   gui_layout[LAYOUT_DT]);
    UpdateRegValue("ST ", &chip8.ST, sizeof(uint8_t), &ST_value, &ST_edit,
                   gui_layout[LAYOUT_ST]);
    GuiLine(gui_layout[LAYOUT_SPE], NULL);
    UpdateRegValue("I ", &chip8.I, sizeof(uint16_t), &I_value, &I_edit,
                   gui_layout[LAYOUT_I]);
    UpdateRegValue("PC ", &chip8.PC, sizeof(uint16_t), &PC_value, &PC_edit,
                   gui_layout[LAYOUT_PC]);
    UpdateRegValue("SP ", &chip8.SP, sizeof(uint16_t), &SP_value, &SP_edit,
                   gui_layout[LAYOUT_SP]);
}

/**
 * @brief Update a single register ValueBox
 * 
 * @param text   Text to display
 * @param reg    Chip-8 register
 * @param size   Size of register (8 or 16 bits)
 * @param value  Displayed value
 * @param edit   Edit mode
 * @param bounds Position and dimensions
 */
void UpdateRegValue(char *text, void *reg, int size, int *value, bool *edit,
                    Rectangle bounds)
{
    if (!(*edit))
    {
        *value = 0;
        memcpy(value, reg, size);
    }

    int max_val = (size == sizeof(uint8_t)) ? 0xFF : 0xFFFF;
    if (GuiValueBox(bounds, text, value, 0x00, max_val, *edit))
    {
        if (*edit) memcpy(reg, value, size);
        *edit = !(*edit);
    }
}

/**
 * @brief Update game view
 * 
 */
void UpdateGameView(void)
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

            if (chip8.display[x + y * CHIP8_DISPLAY_WIDTH])
                DrawRectangleRec(pixel, line_color);
        }
    }
}

/**
 * @brief Update flow control view
 * 
 */
void UpdateFlowView(void)
{
    GuiGroupBox(gui_layout[LAYOUT_FLOW], "Flow control");

    if (GuiButton(gui_layout[LAYOUT_PLAY],
        (play_state) ? GUI_ICON_PAUSE : GUI_ICON_PLAY))
        play_state = !play_state;

    if (GuiButton(gui_layout[LAYOUT_STEP], GUI_ICON_STEP))
        step_state = true;

    if (GuiButton(gui_layout[LAYOUT_RST], GUI_ICON_RST))
        rst_state = true;
}

/**
 * @brief Update Assembly view
 * 
 */
void UpdateASMView(void)
{
    if (rst_state)
    {
        ResetASMView();
        return;
    }

    Rectangle panel = gui_layout[LAYOUT_ASM];
    GuiScrollPanel(panel, NULL, asm_content, &asm_scroll, &asm_view);
    BeginScissorMode(asm_view.x, asm_view.y, asm_view.width, asm_view.height);

    uint16_t opcode = (chip8.ram[chip8.PC] << 8) | chip8.ram[chip8.PC + 1];
    if (opcode != asm_opcode)
    {
        asm_opcode = opcode;
        chip8_op_t parsed;
        chip8_parse(&parsed, opcode);

        static char cmd_str[CHIP8_DECODE_STR_SIZE];
        chip8_decode(NULL, cmd_str, parsed);

        asm_head = (asm_head + 1) % GUI_ASM_BUFFER_LEN;
        sprintf(asm_buffer[asm_head], "0x%04X\t%s", chip8.PC, cmd_str);
        if (asm_counter < GUI_ASM_BUFFER_LEN)
        {
            ++asm_counter;
            if (asm_counter > GUI_ASM_TEXT_LINES)
            {
                asm_content.height += GUI_GRID_SPACING;
                asm_scroll.y -= GUI_GRID_SPACING;
            }
        }
    }

    float text_x = panel.x + asm_scroll.x + GUI_GRID_SPACING;
    float text_y = panel.y + asm_scroll.y + asm_content.height - 38;
    int asm_i = asm_head;

    GuiSetStyle(DEFAULT, TEXT_SIZE, GUI_ASM_FONT_SIZE);
    for (int i = 0; i < asm_counter; ++i)
    {
        Rectangle text_bounds = {text_x, text_y - (GUI_GRID_SPACING * i),
                                 panel.width - GUI_GRID_SPACING * 2,
                                 GUI_GRID_SPACING};
        if (i == 0)
        {
            Rectangle rec_bounds = {GUI_FLOW_X, text_bounds.y, GUI_FLOW_WIDTH,
                                    text_bounds.height};
            DrawRectangleRec(rec_bounds, ColorAlpha(text_color[2], 0.3));
        }

        GuiDrawText(
            asm_buffer[asm_i--],
            text_bounds,
            GUI_GRID_SPACING,
            text_color[0]
        );

        if (asm_i < 0) asm_i += GUI_ASM_BUFFER_LEN;
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
    EndScissorMode();
    DrawRectangleRec((Rectangle){panel.x, panel.y, panel.width - 14, 6},
                     background_color);
    GuiGroupBox(panel, GUI_ASM_TITLE);
}

/**
 * @brief Reset assembly view
 * 
 */
void ResetASMView(void)
{
    memset(asm_buffer, 0, sizeof(asm_buffer));
    memset(&asm_view, 0, sizeof(asm_view));
    memset(&asm_scroll, 0, sizeof(asm_scroll));

    asm_counter = 0;
    asm_head = 0;
    asm_opcode = 0;

    asm_content.x = 0;
    asm_content.y = 0;
    asm_content.width = gui_layout[LAYOUT_ASM].width - 14;
    asm_content.height = gui_layout[LAYOUT_ASM].height;
}

/**
 * @brief Read Chip-8 input keys
 * 
 */
void ReadInputKeys(void)
{
    memset(chip8.keys, 0x00, sizeof(uint8_t) * CHIP8_KEYS_SIZE);
    for (uint8_t i = 0; i < CHIP8_KEYS_SIZE; ++i)
        chip8.keys[i] = IsKeyDown(gui_keys[i]);
}

/**
 * @brief Initialize Cippotto GUI
 * 
 */
void InitCippottoGui(void)
{
    InitWindow(GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, GUI_WINDOW_TITLE);
    SetTargetFPS(CHIP8_CLOCK_HZ);
    GuiLoadStyleCyber();
    GuiSetIconScale(GUI_ICON_SIZE);

    ResetASMView();

    text_size = GuiGetStyle(DEFAULT, TEXT_SIZE);

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
}

/**
 * @brief Update Cippotto GUI
 * 
 */
void UpdateCippottoGui(void)
{
    BeginDrawing();
    ClearBackground(background_color);

    UpdateRegsView();
    UpdateGameView();
    UpdateFlowView();
    UpdateASMView();

    EndDrawing();
}

/**
 * @brief Main function
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return     Return code
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <rom> <addr, optional>\n", argv[0]);
        exit(1);
    }
    char *rom_path = argv[1];
    int rom_addr = (argc > 2) ? atoi(argv[2]) : CHIP8_RAM_PROGRAM;

    FILE *rom = fopen(rom_path, "rb");
    if (rom == NULL)
    {
        fprintf(stderr, "Error opening the ROM file: %s\n", rom_path);
        exit(1);
    }

    chip8_err_t err = CHIP8_OK;
    chip8_cfg_t chip8_cfg = {rom, rom_addr};

    err = chip8_init(&chip8, chip8_cfg);
    if (err == CHIP8_INVALID_ADDR)
    {
        fprintf(stderr, "Invalid ROM address\n");
        exit(1);
    }
    else if (err == CHIP8_INSUFF_MEMORY)
    {
        fprintf(stderr, "Insufficient memory\n");
        exit(1);
    }

    InitCippottoGui();

    while (!WindowShouldClose() && !err)
    {
        if (play_state || step_state)
        {
            err = chip8_run(&chip8);
            step_state = false;
        }

        if (rst_state)
        {
            err = chip8_init(&chip8, chip8_cfg);
            rst_state = false;
        }

        ReadInputKeys();
        UpdateCippottoGui();
    }

    CloseWindow();
    fclose(rom);

    return err;
}

/* -------------------------------------------------------------------------- */
