#pragma once

static HINSTANCE ghInst;

#define NONE				-1
#define XOR_CHANGE_COLOR	0x01
#define XOR_CHANGE_TURN		0x01
#define XOR_CHANGE_SCENE	0x01

#define WINDOW_START_X		500
#define WINDOW_START_Y		200

#define LINE_COUNT			15
#define LINE_INTERVAL		48
#define BOARD_START_POINT	50
#define HALF_LINE_INTERVAL	LINE_INTERVAL / 2.f
#define LINE_WIDTH			1.3f
#define STONE_RADIUS		20.0f

#define RESOLUTION			800

#define DELIM ","

#define BOARD_COLOR 0xF6F2E6

#define BOARD		std::vector<std::vector<eStoneColor>>
#define RENDER_TARGET ID2D1HwndRenderTarget
#define SOLID_BRUSH ID2D1SolidColorBrush

