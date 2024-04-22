#include "pch.h"
#include "eStoneColor.h"
#include "eButton.h"
#include "App.h"
#include "GameManager.h"
#include "SceneManager.h"

namespace gomoku
{
	App* App::mInstance = nullptr;
	GameManager* App::mGameManager = nullptr;
	SceneManager* App::mSceneManager = nullptr;

	/*
		Todo
		* When Search button click search form.
		* Delete mResolution
	*/

	App* App::GetInstance()
	{
		if (mInstance == nullptr)
		{
			ASSERT(mGameManager == nullptr);
			ASSERT(mSceneManager == nullptr);

			mInstance = new App();
		}

		return mInstance;
	}

	HRESULT App::Init(HWND hWnd)
	{
		mhWnd = hWnd;

		mGameManager = GameManager::GetInstance();
		mSceneManager = SceneManager::GetInstance();

		HRESULT hr = mSceneManager->init(hWnd);

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
		mGameManager->release();
		mSceneManager->release();
		
		mGameManager = nullptr;
		mSceneManager = nullptr;

		delete mInstance;
		mInstance = nullptr;
	}

	void App::Run()
	{
		int iResult = mGameManager->BindAndListen();
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
		mSceneManager->render();
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
			if (mGameManager->IsGameOver())
			{
				goto NO_RENDER;
			}

			mGameManager->SetGuideStonePosition(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_LBUTTONDOWN:
			if (mGameManager->IsGameOver())
			{
				goto NO_RENDER;
			}

			mGameManager->AddStone();
			if (mGameManager->IsGameOver())
			{
				mInstance->render();
				mInstance->notifyWinner(mGameManager->GetWinnerStone());
				mGameManager->SetNewGame();
				goto NO_RENDER;
			}
			break;
		case WM_COMMAND:
		{
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
				mSceneManager->changeScene(eScene::Practice);
				break;
			case eButton::Exit:
				mSceneManager->changeScene(eScene::Lobby);
				break;
			}
		}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			goto NO_RENDER;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		  mInstance->render();
	NO_RENDER:
		return 0;
	}
}