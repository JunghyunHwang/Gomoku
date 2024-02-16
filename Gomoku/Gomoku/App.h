#pragma once
#include "GomokuHelper.h"

namespace gomoku
{
	class GameManager;
	enum class eStoneColor;

	class App final
	{
	public:
		static App* GetInstance();
		HRESULT Init(HINSTANCE, HWND, POINT);
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

	private:
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
