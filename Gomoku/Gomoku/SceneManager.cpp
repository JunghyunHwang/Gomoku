#include "pch.h"
#include "SceneManager.h"

namespace gomoku
{
	ID2D1Factory* SceneManager::mD2DFactory = nullptr;
	RENDER_TARGET* SceneManager::mRenderTarget = nullptr;
	SOLID_BRUSH* SceneManager::mBrushes[BRUSH_COUNT] = { nullptr, };

	HWND SceneManager::mhWnd = 0;
	eScene SceneManager::mCurrentScene = eScene::Lobby;
	Scene* SceneManager::mScenes[SCENE_COUNT] = { nullptr, };

	HRESULT SceneManager::init(HWND hWnd)
	{
		ASSERT(hWnd != 0);
		ASSERT(mD2DFactory == nullptr);

		if (mD2DFactory != nullptr)
		{
			return E_FAIL;
		}

		mhWnd = hWnd;

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mD2DFactory);
		if (FAILED(hr))
		{
			return hr;
		}

		// Create device resource
		RECT rt;
		GetClientRect(mhWnd, &rt);
		D2D1_SIZE_U size = D2D1::SizeU(rt.right - rt.left, rt.bottom - rt.top);

		hr = mD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(mhWnd, size),
			&mRenderTarget
		);

		if (FAILED(hr))
		{
			return hr;
		}

		// Failed...
		mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00000000, 1.f), &mBrushes[ENUM_CAST_INT(eBrush::Black)]);
		mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffffff, 1.f), &mBrushes[ENUM_CAST_INT(eBrush::White)]);
		mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00000000, 0.5f), &mBrushes[ENUM_CAST_INT(eBrush::HalfOpacityBlack)]);
		mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffffff, 0.5f), &mBrushes[ENUM_CAST_INT(eBrush::HalfOpacityWhite)]);

		for (size_t i = 0; i < SCENE_COUNT; ++i)
		{
			switch (static_cast<eScene>(i))
			{
			case eScene::Lobby:
			{
				mScenes[i] = new SceneLobby();

				HWND search = CreateWindow(L"button", L"Search", WS_CHILD | WS_VISIBLE, rt.right / 2 - 130, rt.bottom / 2, 100, 30, mhWnd, (HMENU)eButton::Search, ghInst, NULL);
				HWND practice = CreateWindow(L"button", L"혼자 하기", WS_CHILD | WS_VISIBLE, rt.right / 2 + 20, rt.bottom / 2, 100, 30, mhWnd, (HMENU)eButton::Practice, ghInst, NULL);

				mScenes[i]->AddButton(search);
				mScenes[i]->AddButton(practice);
			}
			break;
			case eScene::Practice:
			{
				mScenes[i] = new SceneMatching();

				HWND exit = CreateWindow(L"button", L"나가기", WS_CHILD | WS_VISIBLE, rt.right - 60, rt.bottom - 40, 50, 30, mhWnd, (HMENU)eButton::Exit, ghInst, NULL);
				mScenes[i]->AddButton(exit);
			}
			break;
			case eScene::Matching:
			{
				mScenes[i] = new SceneMatching();

				HWND exit = CreateWindow(L"button", L"나가기", WS_CHILD | WS_VISIBLE, rt.right - 60, rt.bottom - 40, 50, 30, mhWnd, (HMENU)eButton::Exit, ghInst, NULL);
				mScenes[i]->AddButton(exit);
			}
			break;
			default:
				ASSERT(false); // Todo: message
				hr = E_FAIL;
				break;
			}
		}

		return hr;
	}

	void SceneManager::release()
	{
		for (size_t i = 0; i < SCENE_COUNT; ++i)
		{
			delete mScenes[i];
		}

		SafeRelease(&mD2DFactory);
		SafeRelease(&mRenderTarget);

		for (size_t i = 0; i < BRUSH_COUNT; ++i)
		{
			SafeRelease(&mBrushes[i]);
		}
	}
}
