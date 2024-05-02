#pragma once
#include "eScene.h"

/*
	Todo
	* Remove mScene, button, D2D
*/
namespace gomoku
{
	class GameManager;
	class SceneManager;
	enum class eStoneColor;

	class App final
	{
		friend GameManager;
	public:
		static HRESULT Init(HWND, POINT);
		static void Run();
		static void Release();

		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		App() = default;
		~App() = default;
		App(const App* other) = delete;
		App& operator=(const App* rhs) = delete;

		// D2D1
		static void render();
		static void notifyWinner(eStoneColor);

	private:
		static HWND mhWnd;
	};
}
