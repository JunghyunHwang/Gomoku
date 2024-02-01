#include "pch.h"
#include "App.h"
#include "eStoneType.h"

namespace gomoku
{
	App* App::mInstance = nullptr;

	App::App()
		: mHwnd(nullptr)
		, mResolution({ NONE, NONE })
		, mD2DFactory(nullptr)
		, mRenderTarget(nullptr)
		, mGuideStonePosition( { NONE, NONE })
	{
		memset(mBrushes, 0, sizeof(mBrushes));
		memset(mBoard, static_cast<int>(eStoneType::None), sizeof(mBoard));

		mBoard[7][7] = eStoneType::Black; // Test
	}

	App* App::GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new App();
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
			hr = createDeviceREsources();
		}

		return hr;
	}

	HRESULT App::createDeviceREsources()
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
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00000000, 0.f), &mBrushes[0]);
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00000000, 1.f), &mBrushes[1]);
				mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffffff, 1.f), &mBrushes[2]);
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
			auto* emptyBrush = mBrushes[static_cast<int>(eStoneType::None)];
			auto* blackBrush = mBrushes[static_cast<int>(eStoneType::Black)];
			auto* whiteBrush = mBrushes[static_cast<int>(eStoneType::White)];

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

			// Draw Stones
			for (size_t y = 0; y < LINE_COUNT; ++y)
			{
				for (size_t x = 0; x < LINE_COUNT; ++x)
				{
					if (mBoard[y][x] == eStoneType::None)
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

			// Draw GuidStone
			if (isValideGuideLine())
			{
				D2D1_ELLIPSE stoneInfo = D2D1::Ellipse(
					D2D1::Point2F(
						static_cast<float>(BOARD_START_POINT * (mGuideStonePosition.x + 1)),
						static_cast<float>(BOARD_START_POINT * (mGuideStonePosition.y + 1))
					),
					STONE_RADIUS,
					STONE_RADIUS
				);

				mRenderTarget->DrawEllipse(stoneInfo, blackBrush, STONE_STROKE_WIDTH);
				mRenderTarget->FillEllipse(stoneInfo, mBrushes[2]);
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
			goto CALL_RENDER;
		case WM_MOUSEMOVE:
		{
			mInstance->mGuideStonePosition = mInstance->getBoardPosition(LOWORD(lParam), HIWORD(lParam)); // LOWORD(lParam), HIWORD(lParam)
#ifdef _DEBUG
			wchar_t szBuff[50];
			swprintf_s(
				szBuff,
				L"X: %d / Y: %d / Row: %d / Col: %d",
				LOWORD(lParam), HIWORD(lParam), mInstance->mGuideStonePosition.y, mInstance->mGuideStonePosition.x
			);
			SetWindowText(mInstance->mHwnd, szBuff);
#endif
			goto CALL_RENDER;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	CALL_RENDER:
		mInstance->render();

	NO_RENDER:
		return 0;
	}
}