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

		// D2D1
		HRESULT createDeviceResources();
		void render();

		// App
		void setGuideStonePosition(uint32_t x, uint32_t y);
		void addStone();
		bool isValidePosition() const;
		inline void switchTurn();

	private:
		enum
		{
			NONE = -1,
			CURR_STONE_CHANGE = 0x01,
			STONE_STROKE_WIDTH = 2,
			BRUSH_COUNT = 3,
			LINE_COUNT = 15,
			LINE_INTERVAL = 48,
			BOARD_START_POINT = 48,
		};

		static constexpr float HALF_LINE_INTERVAL = LINE_INTERVAL / 2.f;
		static constexpr float LINE_WIDTH = 1.3f;
		static constexpr float STONE_RADIUS = 20.0f;

		static App* mInstance;

		ID2D1Factory* mD2DFactory;
		ID2D1HwndRenderTarget* mRenderTarget;
		ID2D1SolidColorBrush* mBrushes[BRUSH_COUNT];

		HWND mHwnd;
		POINT mResolution;

		eStoneType mBoard[LINE_COUNT][LINE_COUNT];
		eStoneType mCurrentTurnStone;
		POINT mGuideStonePosition;
	};

	void App::switchTurn()
	{
		int curr = static_cast<int>(mCurrentTurnStone);
		mCurrentTurnStone = static_cast<eStoneType>(curr ^ CURR_STONE_CHANGE);
	}
}
