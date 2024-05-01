#pragma once
#include "eScene.h"
#include "GomokuDefines.h"
#include "Macro.h"
#include "eButton.h"
#include "eBrush.h"

#include "Scene.h"
#include "SceneLobby.h"
#include "SceneMatching.h"

/*
	TODO
	* Add window resize
*/

namespace gomoku
{
	class App;
	
	class SceneManager final
	{
		friend App;
		friend SceneLobby;
		friend SceneMatching;
	public:
		static inline const eScene& GetCurrentScene();
	private:
		SceneManager() = delete;
		~SceneManager() = delete;
		SceneManager(const SceneManager& other) = delete;
		SceneManager& operator=(const SceneManager& rhs) = delete;

	private:
		static HRESULT init(HWND);
		static void release();

		static inline void render();
		static inline void changeScene(const eScene& type);

		static inline RENDER_TARGET* getRenderTaget();
		static inline SOLID_BRUSH** getBrushes();

	private:
		enum
		{
			STONE_STROKE_WIDTH = 2,
			LOW_OPACITY = 2,
			BRUSH_COUNT = ENUM_CAST_INT(eBrush::Count),
			SCENE_COUNT = ENUM_CAST_INT(eScene::Count),
		};

		static ID2D1Factory* mD2DFactory;
		static RENDER_TARGET* mRenderTarget;
		static SOLID_BRUSH* mBrushes[BRUSH_COUNT];

		static HWND mhWnd;
		static eScene mCurrentScene;
		static Scene* mScenes[SCENE_COUNT];
	};

	void SceneManager::render()
	{
		mScenes[ENUM_CAST_INT(mCurrentScene)]->render();
	}

	void SceneManager::changeScene(const eScene& type)
	{
		mScenes[ENUM_CAST_INT(mCurrentScene)]->HideButtons();
		mCurrentScene = type;
		mScenes[ENUM_CAST_INT(mCurrentScene)]->ShowButtons();
	}

	RENDER_TARGET* SceneManager::getRenderTaget()
	{
		ASSERT(mRenderTarget != nullptr);
		return mRenderTarget;
	}

	SOLID_BRUSH** SceneManager::getBrushes()
	{
		ASSERT(mRenderTarget != nullptr);
		return mBrushes;
	}

	const eScene& SceneManager::GetCurrentScene()
	{
		return mCurrentScene;
	}
}
