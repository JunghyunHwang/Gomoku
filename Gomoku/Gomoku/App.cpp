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
		Todo: When Search button click search form.
	*/

	HRESULT App::Init(HWND hWnd, POINT resolution)
	{
		mhWnd = hWnd;

		RECT rt = { 0, 0, resolution.x, resolution.y };
		AdjustWindowRect(&rt, WS_OVERLAPPED, false);
		SetWindowPos(mhWnd, nullptr, WINDOW_START_X, WINDOW_START_Y, rt.right - rt.left, rt.bottom - rt.top, 0);

		HRESULT hr = GameManager::init();
		if (FAILED(hr))
		{
			// TODO: When initialization fail
			return hr;
		}

		hr = SceneManager::init(hWnd);
		if (FAILED(hr))
		{
			return hr;
		}

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
			case eScene::Practice:
				GameManager::SetGuideStonePosition(LOWORD(lParam), HIWORD(lParam));
				break;
			case eScene::Lobby:
				return NO_RENDER;
			default:
				ASSERT(false); // Todo: message: Unknown scene type
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
					Todo: Add Search form. mGameManager->ConnectOpponent();
				*/
				MessageBox(hWnd, L"Search Oppnent", L"", MB_OK);
				break;
			case eButton::Practice:
				SceneManager::changeScene(eScene::Practice);
				GameManager::SetNewGame();
				break;
			case eButton::Exit:
				SceneManager::changeScene(eScene::Lobby);
				GameManager::InitGame();
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
