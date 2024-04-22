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
		static App* GetInstance();
		HRESULT Init(HWND);
		void Run();
		void Release();

		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		App() = default;
		~App() = default;
		App(const App* other) = delete;
		App& operator=(const App* rhs) = delete;

		// D2D1
		void render();
		void notifyWinner(eStoneColor);

	private:
		static App* mInstance;
		static GameManager* mGameManager;
		static SceneManager* mSceneManager;

		HWND mhWnd;
	};
}
