#pragma once
#include "eScene.h"
#include "GomokuDefines.h"

/*
	TODO
	* Add window resize
*/

namespace gomoku
{
	class App;
	class GameManager;
	class Scene;
	class SceneLobby;
	class SceneMatching;

	class SceneManager final
	{
		friend App;
		friend SceneLobby;
		friend SceneMatching;
	public:
		static SceneManager* GetInstance();
		
	private:
		SceneManager();
		~SceneManager() = default;

		HRESULT init(HWND);
		void release();
		inline RENDER_TARGET* getRenderTaget();
		inline SOLID_BRUSH** getBrushes();

		void render();
		inline void changeScene(const eScene& type);
		inline const eScene& GetCurrentScene() const;

	private:
		static SceneManager* mInstance;

		enum
		{
			STONE_STROKE_WIDTH = 2,
			LOW_OPACITY = 2,
			BRUSH_COUNT = 4,
			SCENE_COUNT = static_cast<int>(eScene::Count),
		};
		ID2D1Factory* mD2DFactory;
		RENDER_TARGET* mRenderTarget;
		SOLID_BRUSH* mBrushes[BRUSH_COUNT];

		HWND mhWnd;
		Scene* mScenes[SCENE_COUNT];
		eScene mCurrentScene;
	};

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

	const eScene& SceneManager::GetCurrentScene() const
	{
		return mCurrentScene;
	}
}
