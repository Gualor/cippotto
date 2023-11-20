/* Includes ----------------------------------------------------------------- */

#include <raylib.h>
#include <raymath.h>

#include "gui.h"

/* Definitions -------------------------------------------------------------- */

#define GUI_PANEL_NUM 4
#define GUI_PANEL_MAX_SUB 4

/* Data types --------------------------------------------------------------- */

typedef enum
{
    GUI_ID_MAIN,
    GUI_ID_GAME,
    GUI_ID_REGS,
    GUI_ID_OPS
} gui_id_t;

typedef struct gui_panel
{
    gui_id_t id;                              // Unique panel id
    Rectangle rect;                           // Panel relative dimensions
    int n_sub;                                // Number of sub panels
    struct gui_panel *sub[GUI_PANEL_MAX_SUB]; // Array of sub panels
} gui_panel_t;

/* Global variables --------------------------------------------------------- */

static gui_panel_t gui_panel = {
    .id = GUI_ID_MAIN,
    .rect = {0.0f, 0.0f, 1.0f, 1.0f},
    .n_sub = 3,
    .sub = {
        &(gui_panel_t){
            .id = GUI_ID_GAME,
            .rect = {0.0f, 0.0f, 0.5f, 0.5f},
            .n_sub = 0,
        },
        &(gui_panel_t){
            .id = GUI_ID_REGS,
            .rect = {0.0f, 0.5f, 0.5f, 0.5f},
            .n_sub = 0,
        },
        &(gui_panel_t){
            .id = GUI_ID_OPS,
            .rect = {0.5f, 0.0f, 0.5f, 1.0f},
            .n_sub = 0,
        },
    },
};

/* Function prototypes ------------------------------------------------------ */

void gui_draw_game(chip8_t *ch8, Rectangle rect);
void gui_draw_regs(chip8_t *ch8, Rectangle rect);
void gui_draw_ops(chip8_t *ch8, Rectangle rect);

/* Function definitions ----------------------------------------------------- */

void gui_draw(chip8_t *ch8)
{
    BeginDrawing();

    Rectangle parent_rect = {0, 0, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT};
    int n_panels = 1;

    gui_panel_t *panel_list[GUI_PANEL_NUM] = {&gui_panel};

    while (n_panels > 0)
    {
        gui_panel_t *child = panel_list[n_panels - 1];
        --n_panels;

        Rectangle child_rect = {
            parent_rect.x + (child->rect.x * parent_rect.width),
            parent_rect.y + (child->rect.y * parent_rect.height),
            (parent_rect.width * child->rect.width),
            (parent_rect.height * child->rect.height)
        };

        switch (child->id)
        {
        case GUI_ID_MAIN:
            DrawRectangleRec(child_rect, GUI_MAIN_BACK_COLOR);
            break;

        case GUI_ID_GAME:
            gui_draw_game(ch8, child_rect);
            break;

        case GUI_ID_REGS:
            gui_draw_regs(ch8, child_rect);
            break;

        case GUI_ID_OPS:
            gui_draw_ops(ch8, child_rect);
            break;
        }

        if (child->n_sub > 0)
        {
            parent_rect = child_rect;
            for (int i = 0; i < child->n_sub; ++i)
            {
                panel_list[n_panels] = child->sub[i];
                ++n_panels;
            }
        }
    }

    EndDrawing();
}

void gui_draw_game(chip8_t *ch8, Rectangle rect)
{
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
                scale_y
            };

            if (ch8->display[x + y * CHIP8_DISPLAY_WIDTH])
                DrawRectangleRec(pixel, GUI_GAME_DRAW_COLOR);
            else
                DrawRectangleRec(pixel, GUI_GAME_BACK_COLOR);
        }
    }
}

void gui_draw_regs(chip8_t *ch8, Rectangle rect)
{
    float x = rect.x;
    float y = rect.y;

    (void)ch8;
    DrawRectangleRec(rect, GUI_REGS_BACK_COLOR);

    char text[20];
    for (uint8_t i = 0; i < CHIP8_REGS_NUM; ++i)
    {
        sprintf(text, "V%X: %d", i, ch8->V[i]);
        DrawText(text, x, y, 20, GUI_REGS_FONT_COLOR); y += 20.0f;
    }
    sprintf(text, "DT: %d", ch8->DT);
    DrawText(text, x, y, 20, GUI_REGS_FONT_COLOR); y += 20.0f;

    sprintf(text, "ST: %d", ch8->ST);
    DrawText(text, x, y, 20, GUI_REGS_FONT_COLOR); y += 20.0f;

    sprintf(text, "I:  %d", ch8->I);
    DrawText(text, x, y, 20, GUI_REGS_FONT_COLOR); y += 20.0f;

    sprintf(text, "PC: %d", ch8->PC);
    DrawText(text, x, y, 20, GUI_REGS_FONT_COLOR); y += 20.0f;

    sprintf(text, "SP: %d", ch8->SP);
    DrawText(text, x, y, 20, GUI_REGS_FONT_COLOR); y += 20.0f;
}

void gui_draw_ops(chip8_t *ch8, Rectangle rect)
{
    (void)ch8;
    DrawRectangleRec(rect, GUI_CODE_BACK_COLOR);
}

/* -------------------------------------------------------------------------- */
