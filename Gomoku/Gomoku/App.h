#pragma once
#include "pch.h"

namespace gomoku
{
	#define XPOS(x) (BOARD_START_POINT + (x) * LINE_INTERVAL)
	#define YPOS(y) (BOARD_START_POINT + (y) * LINE_INTERVAL)

	enum class eStoneType;

	class App final
	{
	public:
		static App* GetInstance();
		HRESULT Init(HWND, POINT);
		void Run();
		void Release();
		void Resize(uint32_t width, uint32_t height);
		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		App();
		~App() = default;
		App(const App* other) = delete;
		App& operator=(const App* rhs) = delete;

	private:
		HRESULT createDeviceREsources();
		void render();
		inline POINT getBoardPosition(uint32_t x, uint32_t y) const;
		inline bool isValideGuideLine() const;

	private:
		enum
		{
			NONE = -1,
			BOARD_START_POINT = 48,
			LINE_COUNT = 15,
			LINE_INTERVAL = 48,
			BRUSH_COUNT = 3,
			STONE_STROKE_WIDTH = 2,
		};

		static constexpr float HALF_LINE_INTERVAL = LINE_INTERVAL / 2.f;
		static constexpr float LINE_WIDTH = 1.3f;
		static constexpr float STONE_RADIUS = 20.0f;

	private:
		static App* mInstance;

		ID2D1Factory* mD2DFactory;
		ID2D1HwndRenderTarget* mRenderTarget;
		ID2D1SolidColorBrush* mBrushes[BRUSH_COUNT];

		HWND mHwnd;
		POINT mResolution;

		eStoneType mBoard[LINE_COUNT][LINE_COUNT];
		POINT mGuideStonePosition;
	};

	POINT App::getBoardPosition(uint32_t x, uint32_t y) const
	{
		ASSERT(LINE_INTERVAL < 256);
		const int32_t NOMALIZED_X = (x - BOARD_START_POINT);
		const int32_t NOMALIZED_Y = (y - BOARD_START_POINT);

		int8_t col = NOMALIZED_X / LINE_INTERVAL;
		int8_t row = NOMALIZED_Y / LINE_INTERVAL;

		int8_t colAlpha = NOMALIZED_X % LINE_INTERVAL;
		int8_t rowAlpha = NOMALIZED_Y % LINE_INTERVAL;

		colAlpha -= static_cast<int8_t>(HALF_LINE_INTERVAL);
		colAlpha >>= 7;
		colAlpha += 1;

		rowAlpha -= static_cast<int8_t>(HALF_LINE_INTERVAL);
		rowAlpha >>= 7;
		rowAlpha += 1;

		col += colAlpha;
		row += rowAlpha;

		if (col < 0 || col >= LINE_COUNT)
		{
			return { NONE, NONE };
		}

		if (row < 0 || row >= LINE_COUNT)
		{
			return { NONE, NONE };
		}

		return { col, row };
	}

	bool App::isValideGuideLine() const
	{
		return (mGuideStonePosition.x != NONE && mGuideStonePosition.y != NONE);
	}
}
