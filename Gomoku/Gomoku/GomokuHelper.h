#pragma once

namespace gomoku
{
	enum
	{
		NONE = -1,
		CURR_STONE_CHANGE = 0x01,
		STONE_STROKE_WIDTH = 2,
		LOW_OPACITY = 2,
		BRUSH_COUNT = 4,
		MAX_CHAINING_COUNT = 5,
		LINE_COUNT = 15,
		LINE_INTERVAL = 48,
		BOARD_START_POINT = 48,
	};

	#define XPOS(x) (BOARD_START_POINT + (x) * LINE_INTERVAL)
	#define YPOS(y) (BOARD_START_POINT + (y) * LINE_INTERVAL)

	static constexpr float HALF_LINE_INTERVAL = LINE_INTERVAL / 2.f;
	static constexpr float LINE_WIDTH = 1.3f;
	static constexpr float STONE_RADIUS = 20.0f;
}
