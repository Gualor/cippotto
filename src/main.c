/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef  RAYGUI_IMPLEMENTATION
#define GUI_DIALOG_IMPLEMENTATION
#include "gui_dialog.h"
#include "gui_icon.h"
#include "gui_style.h"
#include "gui_const.h"
#include "chip8.h"

/* Function Prototypes ------------------------------------------------------ */

static void ParseArgs(int argc, char **argv);

static void InitEmulator(void);
static void ExecuteEmulator(void);
static void UpdateEmulator(void);

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
static void UpdateROMView(void);

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

// ValueBox: Clock
static int CLK_value = CHIP8_CLOCK_HZ;
static bool CLK_edit = false;

// Button: Play
static bool play_state = false;
static bool start_state = false;

// Button: Step
static bool step_state = false;

// Button: Restart
static bool rst_state = false;

// ScrollPanel: Assembly
static char asm_pc_buffer[GUI_ASM_BUFFER_LEN][7] = {0};
static char asm_op_buffer[GUI_ASM_BUFFER_LEN][CHIP8_DECODE_STR_SIZE] = {0};
static int asm_counter = 0;
static int asm_head = 0;
static int asm_pc = -1;
static Rectangle asm_content = {0};
static Rectangle asm_view = {0};
static Vector2 asm_scroll = {0, 0};

// TextBox: ROM path
static char rom_path[GUI_PATH_MAX] = "";
static bool rom_path_edit = false;

// ValueBox: ROM address
static int rom_addr = CHIP8_RAM_PROGRAM;
static bool rom_addr_edit = false;

// Button: ROM load
static bool rom_load_state = false;
static bool pre_load_state = false;

// Window: File dialog
static GuiWindowFileDialogState file_dialog = {0};

// Style parameters
static Color background_color = {0};
static Color line_color = {0};
static Color text_color[4] = {0};

// Time variables
static double emu_time = 0;
static double gui_time = 0;

// Chip-8 emulator
static chip8_t chip8 = {0};
static chip8_op_t chip8_op = {0};
static chip8_cmd_t chip8_cmd = NULL;

/* Function definitions ----------------------------------------------------- */

/**
 * @brief Update registers view
 * 
 */
void UpdateRegsView(void)
{
    GuiGroupBox(gui_layout[LAYOUT_REGS], "Registers");
    if (!rom_load_state || play_state) GuiDisable();

    // V0-VF registers
    for (uint8_t i = 0; i < CHIP8_REGS_NUM; ++i)
    {
        char Vi[5];
        sprintf(Vi, "V%X ", i);
        UpdateRegValue(Vi, &chip8.V[i], sizeof(uint8_t), &Vi_value[i],
                       &Vi_edit[i], gui_layout[LAYOUT_V0 + i]);
    }

    // Timer registers
    GuiLine(gui_layout[LAYOUT_TIM], NULL);
    UpdateRegValue("DT ", &chip8.DT, sizeof(uint8_t), &DT_value, &DT_edit,
                   gui_layout[LAYOUT_DT]);
    UpdateRegValue("ST ", &chip8.ST, sizeof(uint8_t), &ST_value, &ST_edit,
                   gui_layout[LAYOUT_ST]);

    // Special registers
    GuiLine(gui_layout[LAYOUT_SPE], NULL);
    UpdateRegValue("I ", &chip8.I, sizeof(uint16_t), &I_value, &I_edit,
                   gui_layout[LAYOUT_I]);
    UpdateRegValue("PC ", &chip8.PC, sizeof(uint16_t), &PC_value, &PC_edit,
                   gui_layout[LAYOUT_PC]);
    UpdateRegValue("SP ", &chip8.SP, sizeof(uint16_t), &SP_value, &SP_edit,
                   gui_layout[LAYOUT_SP]);

    // Clock frequency
    GuiDrawText("Clock", gui_layout[LAYOUT_TCLK], TEXT_ALIGN_CENTER,
                text_color[GuiGetState()]);
    if (GuiValueBox(
        gui_layout[LAYOUT_CLK], NULL, &CLK_value, 0, UINT16_MAX, CLK_edit))
        CLK_edit = !CLK_edit;

    if (!rom_load_state || play_state) GuiEnable();
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

    int max_val = (size == sizeof(uint8_t)) ? UINT8_MAX : UINT16_MAX;
    if (GuiValueBox(bounds, text, value, 0, max_val, *edit))
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
    GuiGroupBox(gui_layout[LAYOUT_GAME], "Game");

    Rectangle panel = gui_layout[LAYOUT_DRAW];
    float scale_x = panel.width / CHIP8_DISPLAY_WIDTH;
    float scale_y = panel.height / CHIP8_DISPLAY_HEIGHT;

    for (uint8_t x = 0; x < CHIP8_DISPLAY_WIDTH; ++x)
    {
        for (uint8_t y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y)
        {
            Rectangle pixel = {panel.x + (x * scale_x), panel.y + (y * scale_y),
                               scale_x, scale_y};

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
    if (!rom_load_state) GuiDisable();

    // Play / pause button
    if (GuiButton(
        gui_layout[LAYOUT_PLAY], (play_state) ? GUI_ICON_PAUSE : GUI_ICON_PLAY))
        play_state = !play_state;

    // Step button
    if (play_state) GuiDisable();
    if (GuiButton(gui_layout[LAYOUT_STEP], GUI_ICON_STEP)) step_state = true;
    if (play_state) GuiEnable();

    // Restart button
    if (GuiButton(gui_layout[LAYOUT_RST], GUI_ICON_RST)) rst_state = true;

    if (!rom_load_state) GuiEnable();
}

/**
 * @brief Update Assembly view
 * 
 */
void UpdateASMView(void)
{
    if (rst_state) ResetASMView();

    Rectangle panel = gui_layout[LAYOUT_ASM];
    GuiScrollPanel(panel, NULL, asm_content, &asm_scroll, &asm_view);
    BeginScissorMode(asm_view.x, asm_view.y, asm_view.width, asm_view.height);

    float text_x = panel.x + asm_scroll.x + (GUI_GRID_SPACING * 2);
    float text_y = panel.y + asm_scroll.y + asm_content.height - GUI_GRID_SPACING;
    int asm_i = asm_head;

    Color asm_color = (GuiGetState() == STATE_DISABLED) ?
                       text_color[STATE_DISABLED] : text_color[STATE_PRESSED];

    if (rom_load_state)
    {
        Rectangle PC_rec = {GUI_FLOW_X, text_y, GUI_FLOW_WIDTH, GUI_GRID_SPACING};
        DrawRectangleRec(PC_rec, ColorAlpha(asm_color, 0.3));
    }

    for (int i = 0; i < asm_counter; ++i)
    {
        if (i > 0) asm_color = text_color[STATE_DISABLED];

        Rectangle pc_rec = {text_x, text_y - (GUI_GRID_SPACING * i),
                            GUI_GRID_SPACING * 2, GUI_GRID_SPACING};
        GuiDrawText(asm_pc_buffer[asm_i], pc_rec, TEXT_ALIGN_LEFT, asm_color);

        Rectangle op_rec = {pc_rec.x + pc_rec.width + GUI_GRID_SPACING,
                            pc_rec.y, GUI_GRID_SPACING * 4, GUI_GRID_SPACING};
        GuiDrawText(asm_op_buffer[asm_i], op_rec, TEXT_ALIGN_LEFT, asm_color);

        if (--asm_i < 0) asm_i += GUI_ASM_BUFFER_LEN;
    }

    EndScissorMode();
    DrawRectangleRec((Rectangle){panel.x, panel.y, panel.width - 14, 10},
                     background_color);
    GuiGroupBox(panel, "Assembly");
}

/**
 * @brief Reset assembly view
 * 
 */
void ResetASMView(void)
{
    memset(asm_pc_buffer, 0, sizeof(asm_pc_buffer));
    memset(asm_op_buffer, 0, sizeof(asm_op_buffer));
    memset(&asm_view, 0, sizeof(asm_view));
    memset(&asm_scroll, 0, sizeof(asm_scroll));

    asm_counter = 0;
    asm_head = 0;
    asm_pc = -1;

    asm_content.x = 0;
    asm_content.y = 0;
    asm_content.width = gui_layout[LAYOUT_ASM].width - 14;
    asm_content.height = gui_layout[LAYOUT_ASM].height - 1;
}

/**
 * @brief Update ROM view
 * 
 */
void UpdateROMView(void)
{
    GuiGroupBox(gui_layout[LAYOUT_ROM], "Rom");
    if (play_state || file_dialog.windowActive) GuiDisable();

    GuiDrawText("Rom path", gui_layout[LAYOUT_TROM], TEXT_ALIGN_LEFT,
                text_color[GuiGetState()]);

    // Open file dialog button
    if (GuiButton(gui_layout[LAYOUT_FILE], GUI_ICON_FILE))
        file_dialog.windowActive = true;

    // ROM path search bar
    if (GuiTextBox(gui_layout[LAYOUT_BAR], rom_path, GUI_PATH_MAX, rom_path_edit))
        rom_path_edit = !rom_path_edit;

    // ROM address value box
    if (GuiValueBox(gui_layout[LAYOUT_ADDR], "Load address ", &rom_addr,
        CHIP8_RAM_START, CHIP8_RAM_END, rom_addr_edit))
        rom_addr_edit = !rom_addr_edit;

    // ROM memory load button
    if (GuiButton(gui_layout[LAYOUT_LOAD], "Load") || pre_load_state)
    {
        pre_load_state = false;
        rom_load_state = true;
        play_state = start_state;
        ResetASMView();
        InitEmulator();
    }

    GuiEnable();

    // File dialog window
    GuiWindowFileDialog(&file_dialog);
    if (file_dialog.SelectFilePressed)
    {
        file_dialog.SelectFilePressed = false;
        const char *file_path = TextFormat("%s" PATH_SEPERATOR "%s",
                                           file_dialog.dirPathText,
                                           file_dialog.fileNameText);
        strcpy(rom_path, file_path);
    }

    if (file_dialog.windowActive) GuiDisable();
}

/**
 * @brief Read Chip-8 input keys
 * 
 */
void ReadInputKeys(void)
{
    memset(chip8.keys, 0, sizeof(uint8_t) * CHIP8_KEYS_SIZE);
    for (uint8_t i = 0; i < CHIP8_KEYS_SIZE; ++i)
        chip8.keys[i] = IsKeyDown(gui_keys[i]);
}

/**
 * @brief Initialize Cippotto GUI
 * 
 */
void InitCippottoGui(void)
{
    InitWindow(GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, "Cippotto GUI");
    SetWindowIcon(GUI_ICON_IMAGE);
    GuiLoadStyleCyber();

    file_dialog = InitGuiWindowFileDialog(GetWorkingDirectory());
    file_dialog.windowActive = false;

    ResetASMView();

    background_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    line_color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
    text_color[STATE_NORMAL] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
    text_color[STATE_FOCUSED] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED));
    text_color[STATE_PRESSED] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED));
    text_color[STATE_DISABLED] = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_DISABLED));

    gui_time = GetTime();
}

/**
 * @brief Update Cippotto GUI
 * 
 */
void UpdateCippottoGui(void)
{
    double curr_time = GetTime();
    if (curr_time - gui_time >= 1.0 / GUI_REFRESH_RATE)
    {
        BeginDrawing();
        ClearBackground(background_color);

        UpdateRegsView();
        UpdateGameView();
        UpdateFlowView();
        UpdateASMView();
        UpdateROMView();

        EndDrawing();

        gui_time = curr_time;
    }
}

/**
 * @brief Initialize Chip-8 emulator
 * 
 */
void InitEmulator(void)
{
    FILE *rom = fopen(rom_path, "rb");
    if (rom == NULL)
    {
        fprintf(stderr, "Error opening the ROM file: %s\n", rom_path);
        exit(1);
    }

    chip8_cfg_t chip8_cfg = {rom, rom_addr};
    chip8_err_t err = chip8_init(&chip8, chip8_cfg);
    if (err)
    {
        if (err == CHIP8_INVALID_ADDR)
            fprintf(stderr, "Invalid ROM address\n");
        else if (err == CHIP8_INSUFF_MEMORY)
            fprintf(stderr, "Insufficient memory\n");
        exit(err);
    }
    fclose(rom);

    UpdateEmulator();

    emu_time = GetTime();
}

/**
 * @brief Update Chip-8 emulator
 * 
 */
void UpdateEmulator(void)
{
    asm_head = (asm_head + 1) % GUI_ASM_BUFFER_LEN;
    sprintf(asm_pc_buffer[asm_head], "0x%04X", chip8.PC);

    chip8_fetch(&chip8, &chip8_op);
    chip8_decode(&chip8_cmd, asm_op_buffer[asm_head], chip8_op);

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

/**
 * @brief Execute Chip-8 emulator
 * 
 */
void ExecuteEmulator(void)
{
    if (rst_state)
    {
        InitEmulator();
        rst_state = false;
        return;
    }

    double curr_time = GetTime();
    if (play_state || step_state)
    {
        if (curr_time - emu_time >= 1.0 / CLK_value)
        {
            chip8_err_t err = chip8_execute(&chip8, chip8_cmd, chip8_op);
            if (err)
            {
                fprintf(stderr, "Emulator runtime error\n");
                exit(err);
            }

            UpdateEmulator();

            step_state = false;
            emu_time = curr_time;
        }
    }
}

/**
 * @brief Parse command line arguments
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 */
void ParseArgs(int argc, char **argv)
{
    bool arg_rom = false;
    bool arg_flag = false;

    int arg_i = 0;
    while (++arg_i < argc)
    {
        if (!strcmp(argv[arg_i], "-h") || !strcmp(argv[arg_i], "--help"))
        {
            printf(
                "Usage: %s [options]\n"
                "Options:\n"
                "    -h, --help         Print this message and exit.\n"
                "    -r, --rom <file>   Specify Chip-8 ROM file path.\n"
                "    -a, --addr <NNN>   Specify 12-bit ROM loading address.\n"
                "                       If both -r and -a are specified the ROM is pre-loaded.\n"
                "    -s, --start        Start game automatically upon loading.\n",
                argv[0]
            );
            exit(0);
        }
        else if (!strcmp(argv[arg_i], "-r") || !strcmp(argv[arg_i], "--rom"))
        {
            arg_rom = true;
            strcpy(rom_path, argv[++arg_i]);
        }
        else if (!strcmp(argv[arg_i], "-a") || !strcmp(argv[arg_i], "--addr"))
        {
            arg_flag = true;
            rom_addr = atoi(argv[++arg_i]);
        }
        else if (!strcmp(argv[arg_i], "-s") || !strcmp(argv[arg_i], "--start"))
        {
            start_state = true;
        }
        else
        {
            fprintf(stderr, "Unrecognized argument %s\n", argv[arg_i]);
            exit(1);
        }
    }

    if (arg_rom && arg_flag) pre_load_state = true;
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
    ParseArgs(argc, argv);

    InitCippottoGui();

    while (!WindowShouldClose())
    {
        ReadInputKeys();
        ExecuteEmulator();
        UpdateCippottoGui();
    }

    CloseWindow();

    return 0;
}

/* -------------------------------------------------------------------------- */
