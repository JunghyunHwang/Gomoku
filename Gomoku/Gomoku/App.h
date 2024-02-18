#pragma once
#include "GomokuHelper.h"

namespace gomoku
{
	class GameManager;
	enum class eStoneColor;

	class App final
	{
		friend GameManager;
	public:
		static App* GetInstance();
		HRESULT Init(HINSTANCE, HWND, POINT);
		void Run();
		void Release();
		void Resize(uint32_t, uint32_t);

		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		App();
		~App() = default;
		App(const App* other) = delete;
		App& operator=(const App* rhs) = delete;

		// D2D1
		HRESULT createDeviceResources();
		void render();
		void notifyWinner(eStoneColor);

	private:
		enum
		{
			STONE_STROKE_WIDTH = 2,
			LOW_OPACITY = 2,
			BRUSH_COUNT = 4,
		};

		static App* mInstance;
		static GameManager* mGameManager;

		ID2D1Factory* mD2DFactory;
		ID2D1HwndRenderTarget* mRenderTarget;
		ID2D1SolidColorBrush* mBrushes[BRUSH_COUNT];

		HINSTANCE mHinst;
		HWND mHwnd;
		POINT mResolution;
	};
}
