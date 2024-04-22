#pragma once
#include "GomokuDefines.h"
#include "SceneManager.h"

namespace gomoku
{
	class SceneManager;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene& operator=(const Scene& rhs) = delete;

		virtual void Render() = 0;

		inline void AddButton(const HWND& button);
		void ShowButtons() const;
		void HideButtons() const;

	protected:
		void redrawButtons() const;

	protected:
		static SceneManager* mSceneManager;
		std::vector<HWND> mButtons;
	};

	void Scene::AddButton(const HWND& button)
	{
		mButtons.push_back(button);
	}
}
