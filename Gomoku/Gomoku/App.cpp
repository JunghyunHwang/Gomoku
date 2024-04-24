#include "pch.h"
#include "eStoneColor.h"
#include "eButton.h"
#include "App.h"
#include "GameManager.h"
#include "SceneManager.h"

namespace gomoku
{
	HWND App::mhWnd = 0;

	/*
		Todo
		* How to make singleton class in cpp
		* When Search button click search form.
		* Delete mResolution
	*/

	HRESULT App::Init(HWND hWnd)
	{
		mhWnd = hWnd;

		HRESULT hr = GameManager::init();
		if (FAILED(hr))
		{
			// TODO: When initialization fail
			return hr;
		}

		hr = SceneManager::init(hWnd);
		return hr;
	}
	
	void App::notifyWinner(eStoneColor winner)
	{
		switch (winner)
		{
		case eStoneColor::Black:
			MessageBox(mhWnd, L"Black wins", L"Congratulations!", MB_OK);
			break;
		case eStoneColor::White:
			MessageBox(mhWnd, L"White wins", L"Congratulations!", MB_OK);
			break;
		}
	}

	void App::Release()
	{
		GameManager::release();
		SceneManager::release();
	}

	void App::Run()
	{
		int iResult = GameManager::BindAndListen();
		if (iResult != 0)
		{
			std::cout << "ERROR: Failed to Bind with error code: " << iResult << std::endl;
			return;
		}

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void App::render()
	{
		SceneManager::render();
	}

	LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
			ValidateRect(hWnd, NULL);
			break;
		case WM_MOUSEMOVE:
		{
			switch (SceneManager::GetCurrentScene())
			{
			case eScene::Matching:
				GameManager::SetGuideStonePosition(LOWORD(lParam), HIWORD(lParam));
				break;
			case eScene::Lobby:
				return NO_RENDER;
			default:
				ASSERT(false); // message: Unknown scene type
				return NO_RENDER;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			if (GameManager::IsGameOver())
			{
				return NO_RENDER;
			}

			GameManager::AddStone();
			if (GameManager::IsGameOver())
			{
				render();
				notifyWinner(GameManager::GetWinnerStone());
				GameManager::SetNewGame();
				return NO_RENDER;
			}
			break;
		case WM_COMMAND:
			switch (static_cast<eButton>(wParam))
			{
			case eButton::Search:
				/*
					Todo
					* Add Search form. mGameManager->ConnectOpponent();
				*/
				MessageBox(hWnd, L"Search Oppnent", L"", MB_OK);
				break;
			case eButton::Practice:
				GameManager::SetNewGame();
				SceneManager::changeScene(eScene::Matching);
				break;
			case eButton::Exit:
				GameManager::InitGame();
				SceneManager::changeScene(eScene::Lobby);
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return NO_RENDER;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		render();
		return 0;
	}
}