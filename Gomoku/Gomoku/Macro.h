#pragma once

#define XPOS(x) (BOARD_START_POINT + (x) * LINE_INTERVAL)
#define YPOS(y) (BOARD_START_POINT + (y) * LINE_INTERVAL)

#define ENUM_CAST_INT(num) static_cast<int>((num))