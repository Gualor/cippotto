/* Header Guard ------------------------------------------------------------- */

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

/* Definitions -------------------------------------------------------------- */

#define GUI_WINDOW_TITLE  "Cippotto GUI"
#define GUI_WINDOW_WIDTH  1520
#define GUI_WINDOW_HEIGHT 552

#define GUI_GRID_SPACING  24

#define GUI_REGS_TITLE   "Registers"
#define GUI_REGS_X       (GUI_GRID_SPACING)
#define GUI_REGS_Y       (GUI_GRID_SPACING)
#define GUI_REGS_WIDTH   (GUI_GRID_SPACING * 11)
#define GUI_REGS_HEIGHT  (GUI_WINDOW_HEIGHT - (GUI_GRID_SPACING * 2))
#define GUI_REGS_SPACING 32

#define GUI_V0_X      (GUI_REGS_X + GUI_GRID_SPACING * 2)
#define GUI_V0_Y      (GUI_REGS_Y + GUI_GRID_SPACING)
#define GUI_V0_WIDTH  (GUI_GRID_SPACING * 3)
#define GUI_V0_HEIGHT (GUI_GRID_SPACING)

#define GUI_V1_X      (GUI_V0_X)
#define GUI_V1_Y      (GUI_V0_Y + GUI_REGS_SPACING)
#define GUI_V1_WIDTH  (GUI_V0_WIDTH)
#define GUI_V1_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V2_X      (GUI_V0_X)
#define GUI_V2_Y      (GUI_V1_Y + GUI_REGS_SPACING)
#define GUI_V2_WIDTH  (GUI_V0_WIDTH)
#define GUI_V2_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V3_X      (GUI_V0_X)
#define GUI_V3_Y      (GUI_V2_Y + GUI_REGS_SPACING)
#define GUI_V3_WIDTH  (GUI_V0_WIDTH)
#define GUI_V3_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V4_X      (GUI_V0_X)
#define GUI_V4_Y      (GUI_V3_Y + GUI_REGS_SPACING)
#define GUI_V4_WIDTH  (GUI_V0_WIDTH)
#define GUI_V4_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V5_X      (GUI_V0_X)
#define GUI_V5_Y      (GUI_V4_Y + GUI_REGS_SPACING)
#define GUI_V5_WIDTH  (GUI_V0_WIDTH)
#define GUI_V5_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V6_X      (GUI_V0_X)
#define GUI_V6_Y      (GUI_V5_Y + GUI_REGS_SPACING)
#define GUI_V6_WIDTH  (GUI_V0_WIDTH)
#define GUI_V6_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V7_X      (GUI_V0_X)
#define GUI_V7_Y      (GUI_V6_Y + GUI_REGS_SPACING)
#define GUI_V7_WIDTH  (GUI_V0_WIDTH)
#define GUI_V7_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V8_X      (GUI_REGS_X + (GUI_GRID_SPACING * 7))
#define GUI_V8_Y      (GUI_V0_Y)
#define GUI_V8_WIDTH  (GUI_V0_WIDTH)
#define GUI_V8_HEIGHT (GUI_V0_HEIGHT)

#define GUI_V9_X      (GUI_V8_X)
#define GUI_V9_Y      (GUI_V1_Y)
#define GUI_V9_WIDTH  (GUI_V0_WIDTH)
#define GUI_V9_HEIGHT (GUI_V0_HEIGHT)

#define GUI_VA_X      (GUI_V8_X)
#define GUI_VA_Y      (GUI_V2_Y)
#define GUI_VA_WIDTH  (GUI_V0_WIDTH)
#define GUI_VA_HEIGHT (GUI_V0_HEIGHT)

#define GUI_VB_X      (GUI_V8_X)
#define GUI_VB_Y      (GUI_V3_Y)
#define GUI_VB_WIDTH  (GUI_V0_WIDTH)
#define GUI_VB_HEIGHT (GUI_V0_HEIGHT)

#define GUI_VC_X      (GUI_V8_X)
#define GUI_VC_Y      (GUI_V4_Y)
#define GUI_VC_WIDTH  (GUI_V0_WIDTH)
#define GUI_VC_HEIGHT (GUI_V0_HEIGHT)

#define GUI_VD_X      (GUI_V8_X)
#define GUI_VD_Y      (GUI_V5_Y)
#define GUI_VD_WIDTH  (GUI_V0_WIDTH)
#define GUI_VD_HEIGHT (GUI_V0_HEIGHT)

#define GUI_VE_X      (GUI_V8_X)
#define GUI_VE_Y      (GUI_V6_Y)
#define GUI_VE_WIDTH  (GUI_V0_WIDTH)
#define GUI_VE_HEIGHT (GUI_V0_HEIGHT)

#define GUI_VF_X      (GUI_V8_X)
#define GUI_VF_Y      (GUI_V7_Y)
#define GUI_VF_WIDTH  (GUI_V0_WIDTH)
#define GUI_VF_HEIGHT (GUI_V0_HEIGHT)

#define GUI_TIM_X      (GUI_REGS_X + GUI_GRID_SPACING)
#define GUI_TIM_Y      (GUI_V7_Y + 40)
#define GUI_TIM_WIDTH  (GUI_REGS_WIDTH - (GUI_GRID_SPACING * 2))
#define GUI_TIM_HEIGHT 16

#define GUI_DT_X      (GUI_V0_X)
#define GUI_DT_Y      (GUI_TIM_Y + GUI_REGS_SPACING)
#define GUI_DT_WIDTH  (GUI_V0_WIDTH)
#define GUI_DT_HEIGHT (GUI_V0_HEIGHT)

#define GUI_ST_X      (GUI_V8_X)
#define GUI_ST_Y      (GUI_DT_Y)
#define GUI_ST_WIDTH  (GUI_V0_WIDTH)
#define GUI_ST_HEIGHT (GUI_V0_HEIGHT)

#define GUI_SPE_X      (GUI_TIM_X)
#define GUI_SPE_Y      (GUI_DT_Y + 40)
#define GUI_SPE_WIDTH  (GUI_TIM_WIDTH)
#define GUI_SPE_HEIGHT (GUI_TIM_HEIGHT)

#define GUI_I_X      (GUI_V0_X)
#define GUI_I_Y      (GUI_SPE_Y + GUI_REGS_SPACING)
#define GUI_I_WIDTH  (GUI_V0_WIDTH)
#define GUI_I_HEIGHT (GUI_V0_HEIGHT)

#define GUI_PC_X      (GUI_V0_X)
#define GUI_PC_Y      (GUI_I_Y + GUI_REGS_SPACING)
#define GUI_PC_WIDTH  (GUI_V0_WIDTH)
#define GUI_PC_HEIGHT (GUI_V0_HEIGHT)

#define GUI_SP_X      (GUI_V0_X)
#define GUI_SP_Y      (GUI_PC_Y + GUI_REGS_SPACING)
#define GUI_SP_WIDTH  (GUI_V0_WIDTH)
#define GUI_SP_HEIGHT (GUI_V0_HEIGHT)

#define GUI_GAME_TITLE  "Game"
#define GUI_GAME_X      (GUI_REGS_X + GUI_REGS_WIDTH + GUI_GRID_SPACING)
#define GUI_GAME_Y      (GUI_REGS_Y)
#define GUI_GAME_WIDTH  896
#define GUI_GAME_HEIGHT (GUI_REGS_HEIGHT)

#define GUI_DRAW_X      (GUI_GAME_X + GUI_GRID_SPACING)
#define GUI_DRAW_Y      (GUI_GAME_Y + GUI_GRID_SPACING)
#define GUI_DRAW_WIDTH  (GUI_GAME_WIDTH - (GUI_GRID_SPACING * 2))
#define GUI_DRAW_HEIGHT (GUI_GAME_HEIGHT - (GUI_GRID_SPACING * 2))

#define GUI_FLOW_TITLE  "Flow control"
#define GUI_FLOW_X      (GUI_GAME_X + GUI_GAME_WIDTH + GUI_GRID_SPACING)
#define GUI_FLOW_Y      (GUI_REGS_Y)
#define GUI_FLOW_WIDTH  (GUI_REGS_WIDTH)
#define GUI_FLOW_HEIGHT (GUI_GRID_SPACING * 4)

#define GUI_PLAY_X      (GUI_FLOW_X + GUI_GRID_SPACING)
#define GUI_PLAY_Y      (GUI_FLOW_Y + GUI_GRID_SPACING)
#define GUI_PLAY_WIDTH  (GUI_GRID_SPACING * 2)
#define GUI_PLAY_HEIGHT (GUI_GRID_SPACING * 2)
#define GUI_PLAY_PIXELS 3

#define GUI_STEP_X      (GUI_FLOW_X + (GUI_FLOW_WIDTH - GUI_STEP_WIDTH) / 2)
#define GUI_STEP_Y      (GUI_PLAY_Y)
#define GUI_STEP_WIDTH  (GUI_GRID_SPACING * 2)
#define GUI_STEP_HEIGHT (GUI_GRID_SPACING * 2)
#define GUI_STEP_PIXELS (GUI_PLAY_PIXELS)

#define GUI_RST_X      (GUI_FLOW_X + GUI_FLOW_WIDTH - GUI_RST_WIDTH - GUI_GRID_SPACING)
#define GUI_RST_Y      (GUI_PLAY_Y)
#define GUI_RST_WIDTH  (GUI_GRID_SPACING * 2)
#define GUI_RST_HEIGHT (GUI_GRID_SPACING * 2)
#define GUI_RST_PIXELS (GUI_PLAY_PIXELS)

#define GUI_ASM_TITLE      "Assembly"
#define GUI_ASM_X          (GUI_FLOW_X)
#define GUI_ASM_Y          (GUI_FLOW_Y + GUI_FLOW_HEIGHT + GUI_GRID_SPACING)
#define GUI_ASM_WIDTH      (GUI_REGS_WIDTH)
#define GUI_ASM_HEIGHT     (GUI_REGS_HEIGHT - GUI_FLOW_HEIGHT - GUI_GRID_SPACING)
#define GUI_ASM_TEXT_LINES ((GUI_ASM_HEIGHT / GUI_GRID_SPACING) - 1)
#define GUI_ASM_FONT_SIZE  22
#define GUI_ASM_BUFFER_LEN 100

#define GUI_ICON_PLAY  "#131#"
#define GUI_ICON_PAUSE "#132#"
#define GUI_ICON_STEP  "#208#"
#define GUI_ICON_RST   "#211#"
#define GUI_ICON_SIZE  3

/* Data types --------------------------------------------------------------- */

typedef enum {
    LAYOUT_REGS, // GroupBox: Registers
    LAYOUT_V0,   // ValueBox: V0
    LAYOUT_V1,   // ValueBox: V1
    LAYOUT_V2,   // ValueBox: V2
    LAYOUT_V3,   // ValueBox: V3
    LAYOUT_V4,   // ValueBox: V4
    LAYOUT_V5,   // ValueBox: V5
    LAYOUT_V6,   // ValueBox: V6
    LAYOUT_V7,   // ValueBox: V7
    LAYOUT_V8,   // ValueBox: V8
    LAYOUT_V9,   // ValueBox: V9
    LAYOUT_VA,   // ValueBox: VA
    LAYOUT_VB,   // ValueBox: VB
    LAYOUT_VC,   // ValueBox: VC
    LAYOUT_VD,   // ValueBox: VD
    LAYOUT_VE,   // ValueBox: VE
    LAYOUT_VF,   // ValueBox: VF
    LAYOUT_TIM,  // Line:     Timers
    LAYOUT_DT,   // ValueBox: DT
    LAYOUT_ST,   // ValueBox: ST
    LAYOUT_SPE,  // Line:     Special
    LAYOUT_I,    // ValueBox: I
    LAYOUT_PC,   // ValueBox: PC
    LAYOUT_SP,   // ValueBox: SP
    LAYOUT_GAME, // GroupBox: Game
    LAYOUT_DRAW, // Panel:    Draw
    LAYOUT_FLOW, // GroupBox: Flow control
    LAYOUT_PLAY, // Button:   Play
    LAYOUT_STEP, // Button:   Step
    LAYOUT_RST,  // Button:   Restart
    LAYOUT_ASM,  // GroupBox: Assembly
} layout_id_t;

/* Global variables --------------------------------------------------------- */

static const Rectangle gui_layout[] = {
    (Rectangle){ GUI_REGS_X, GUI_REGS_Y, GUI_REGS_WIDTH, GUI_REGS_HEIGHT },
    (Rectangle){ GUI_V0_X,   GUI_V0_Y,   GUI_V0_WIDTH,   GUI_V0_HEIGHT   },
    (Rectangle){ GUI_V1_X,   GUI_V1_Y,   GUI_V1_WIDTH,   GUI_V1_HEIGHT   },
    (Rectangle){ GUI_V2_X,   GUI_V2_Y,   GUI_V2_WIDTH,   GUI_V2_HEIGHT   },
    (Rectangle){ GUI_V3_X,   GUI_V3_Y,   GUI_V3_WIDTH,   GUI_V3_HEIGHT   },
    (Rectangle){ GUI_V4_X,   GUI_V4_Y,   GUI_V4_WIDTH,   GUI_V4_HEIGHT   },
    (Rectangle){ GUI_V5_X,   GUI_V5_Y,   GUI_V5_WIDTH,   GUI_V5_HEIGHT   },
    (Rectangle){ GUI_V6_X,   GUI_V6_Y,   GUI_V6_WIDTH,   GUI_V6_HEIGHT   },
    (Rectangle){ GUI_V7_X,   GUI_V7_Y,   GUI_V7_WIDTH,   GUI_V7_HEIGHT   },
    (Rectangle){ GUI_V8_X,   GUI_V8_Y,   GUI_V8_WIDTH,   GUI_V8_HEIGHT   },
    (Rectangle){ GUI_V9_X,   GUI_V9_Y,   GUI_V9_WIDTH,   GUI_V9_HEIGHT   },
    (Rectangle){ GUI_VA_X,   GUI_VA_Y,   GUI_VA_WIDTH,   GUI_VA_HEIGHT   },
    (Rectangle){ GUI_VB_X,   GUI_VB_Y,   GUI_VB_WIDTH,   GUI_VB_HEIGHT   },
    (Rectangle){ GUI_VC_X,   GUI_VC_Y,   GUI_VC_WIDTH,   GUI_VC_HEIGHT   },
    (Rectangle){ GUI_VD_X,   GUI_VD_Y,   GUI_VD_WIDTH,   GUI_VD_HEIGHT   },
    (Rectangle){ GUI_VE_X,   GUI_VE_Y,   GUI_VE_WIDTH,   GUI_VE_HEIGHT   },
    (Rectangle){ GUI_VF_X,   GUI_VF_Y,   GUI_VF_WIDTH,   GUI_VF_HEIGHT   },
    (Rectangle){ GUI_TIM_X,  GUI_TIM_Y,  GUI_TIM_WIDTH,  GUI_TIM_HEIGHT  },
    (Rectangle){ GUI_DT_X,   GUI_DT_Y,   GUI_DT_WIDTH,   GUI_DT_HEIGHT   },
    (Rectangle){ GUI_ST_X,   GUI_ST_Y,   GUI_ST_WIDTH,   GUI_ST_HEIGHT   },
    (Rectangle){ GUI_SPE_X,  GUI_SPE_Y,  GUI_SPE_WIDTH,  GUI_SPE_HEIGHT  },
    (Rectangle){ GUI_I_X,    GUI_I_Y,    GUI_I_WIDTH,    GUI_I_HEIGHT    },
    (Rectangle){ GUI_PC_X,   GUI_PC_Y,   GUI_PC_WIDTH,   GUI_PC_HEIGHT   },
    (Rectangle){ GUI_SP_X,   GUI_SP_Y,   GUI_SP_WIDTH,   GUI_SP_HEIGHT   },
    (Rectangle){ GUI_GAME_X, GUI_GAME_Y, GUI_GAME_WIDTH, GUI_GAME_HEIGHT },
    (Rectangle){ GUI_DRAW_X, GUI_DRAW_Y, GUI_DRAW_WIDTH, GUI_DRAW_HEIGHT },
    (Rectangle){ GUI_FLOW_X, GUI_FLOW_Y, GUI_FLOW_WIDTH, GUI_FLOW_HEIGHT },
    (Rectangle){ GUI_PLAY_X, GUI_PLAY_Y, GUI_PLAY_WIDTH, GUI_PLAY_HEIGHT },
    (Rectangle){ GUI_STEP_X, GUI_STEP_Y, GUI_STEP_WIDTH, GUI_STEP_HEIGHT },
    (Rectangle){ GUI_RST_X,  GUI_RST_Y,  GUI_RST_WIDTH,  GUI_RST_HEIGHT  },
    (Rectangle){ GUI_ASM_X,  GUI_ASM_Y,  GUI_ASM_WIDTH,  GUI_ASM_HEIGHT  },
};

static const int gui_keys[] = {
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

#endif /* __CONSTANTS_H__ */

/* -------------------------------------------------------------------------- */
