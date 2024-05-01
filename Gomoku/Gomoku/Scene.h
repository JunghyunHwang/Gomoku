#pragma once
#include "GomokuDefines.h"

namespace gomoku
{
	class SceneManager;

	class Scene
	{
		friend SceneManager;
	public:
		Scene();
		~Scene() = default;
		Scene(const Scene& other) = default;
		Scene& operator=(const Scene& rhs) = default;

		inline void AddButton(const HWND& button);
		void ShowButtons() const;
		void HideButtons() const;

	protected:
		virtual void render() = 0;
		void redrawButtons() const;

	protected:
		std::vector<HWND> mButtons;
	};

	void Scene::AddButton(const HWND& button)
	{
		mButtons.push_back(button);
	}
}
