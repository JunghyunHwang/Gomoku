#include "pch.h"
#include "eStoneType.h"
#include "App.h"
#include "GameManager.h"

namespace gomoku
{
	App* App::mInstance = nullptr;
	GameManager* App::mGameManager = nullptr;

	App::App()
		: mHwnd(nullptr)
		, mResolution({ NONE, NONE })
		, mD2DFactory(nullptr)
		, mRenderTarget(nullptr)
	{
	}

	App* App::GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new App();
			mGameManager = GameManager::GetInstance();
		}

		return mInstance;
	}

	HRESULT App::Init(HWND hWnd, POINT resolution)
	{
		ASSERT(hWnd != NULL);
		ASSERT(mD2DFactory == nullptr);
		if (mD2DFactory != nullptr)
		{
			return E_FAIL;
		}

		mHwnd = hWnd;
		mResolution = resolution;

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mD2DFactory);
		if (SUCCEEDED(hr))
		{
			hr = createDeviceResources();
		}

		return hr;
	}

	HRESULT App::createDeviceResources()
	{
		ASSERT(mHwnd != NULL);
		HRESULT hr = S_OK;

		if (mRenderTarget == nullptr)
		{
			RECT rt;
			GetClientRect(mHwnd, &rt);

			D2D1_SIZE_U size = D2D1::SizeU(rt.right - rt.left, rt.bottom - rt.top);

			hr = mD2DFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(mHwnd, size),
				&mRenderTarget
			);

			if (SUCCEEDED(hr))
			{
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00000000, 1.f), &mBrushes[0]); // Black
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffffff, 1.f), &mBrushes[1]); // White
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00000000, 0.5f), &mBrushes[2]); // Low opacity black
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffffff, 0.5f), &mBrushes[3]); // Low opacity white
			}
		}

		return hr;
	}

	void App::Resize(uint32_t width, uint32_t height)
	{
		ASSERT(mRenderTarget != nullptr);
		if (mRenderTarget != nullptr)
		{
			mRenderTarget->Resize({ width, height });
		}
	}

	void App::Release()
	{
		SafeRelease(&mD2DFactory);
		SafeRelease(&mRenderTarget);

		for (size_t i = 0; i < BRUSH_COUNT; ++i)
		{
			SafeRelease(&mBrushes[i]);
		}

		delete mInstance;
	}

	void App::Run()
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void App::render()
	{
		ASSERT(mRenderTarget != nullptr);
		mRenderTarget->BeginDraw();
		{
			mRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			mRenderTarget->Clear(D2D1::ColorF(0xf5ad42));


			// Draw vertical line
			auto* blackBrush = mBrushes[static_cast<int>(eStoneColor::Black)];
			auto* lowOpacityBlackBrush = mBrushes[static_cast<int>(eStoneColor::Black) + LOW_OPACITY];

			for (size_t x = 0; x < LINE_COUNT; ++x)
			{
				mRenderTarget->DrawLine(
					D2D1::Point2F(static_cast<float>(XPOS(x)), BOARD_START_POINT),
					D2D1::Point2F(static_cast<float>(XPOS(x)), YPOS(LINE_COUNT - 1)),
					blackBrush,
					static_cast<float>(LINE_WIDTH)
				);
			}

			// Draw horizontal line
			for (size_t y = 0; y < LINE_COUNT; ++y)
			{
				mRenderTarget->DrawLine(
					D2D1::Point2F(BOARD_START_POINT, static_cast<float>(YPOS(y))),
					D2D1::Point2F(XPOS(LINE_COUNT - 1), static_cast<float>(YPOS(y))),
					blackBrush,
					static_cast<float>(LINE_WIDTH)
				);
			}

			auto mBoard = mGameManager->GetBoard();

			// Draw GuidStone
			if (mGameManager->IsValidePosition())
			{
				eStoneColor currStone = mGameManager->GetCurrentStoneColor();
				POINT currPos = mGameManager->GetGuideStonePosition();

				D2D1_ELLIPSE guideStoneInfo = D2D1::Ellipse(
					D2D1::Point2F(
						static_cast<float>(BOARD_START_POINT * (currPos.x + 1)),
						static_cast<float>(BOARD_START_POINT * (currPos.y + 1))
					),
					STONE_RADIUS,
					STONE_RADIUS
				);

				mRenderTarget->DrawEllipse(guideStoneInfo, lowOpacityBlackBrush, STONE_STROKE_WIDTH);
				mRenderTarget->FillEllipse(guideStoneInfo, mBrushes[static_cast<int>(currStone) + LOW_OPACITY]);
			}

			// Draw Board
			for (size_t y = 0; y < LINE_COUNT; ++y)
			{
				for (size_t x = 0; x < LINE_COUNT; ++x)
				{
					if (mBoard[y][x] == eStoneColor::None)
					{
						continue;
					}

					D2D1_ELLIPSE stoneInfo = D2D1::Ellipse(
						D2D1::Point2F(
							static_cast<float>(BOARD_START_POINT * (x + 1)),
							static_cast<float>(BOARD_START_POINT * (y + 1))
						),
						STONE_RADIUS,
						STONE_RADIUS
					);

					mRenderTarget->DrawEllipse(stoneInfo, blackBrush, STONE_STROKE_WIDTH);
					mRenderTarget->FillEllipse(stoneInfo, mBrushes[static_cast<int>(mBoard[y][x])]);
				}
			}
		}
		mRenderTarget->EndDraw();
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
			mGameManager->SetGuideStonePosition(LOWORD(lParam), HIWORD(lParam));
#ifdef _DEBUG
			wchar_t szBuff[50];
			POINT p = mGameManager->GetGuideStonePosition();
			swprintf_s(
				szBuff,
				L"X: %d / Y: %d / Row: %d / Col: %d",
				LOWORD(lParam), HIWORD(lParam), p.y, p.x
			);
			SetWindowText(mInstance->mHwnd, szBuff);
#endif
			break;
		}
		case WM_LBUTTONDOWN:
			mGameManager->AddStone();
			mGameManager->IsGameOver();
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