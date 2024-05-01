#include "pch.h"
#include "eButton.h"
#include "Scene.h"

namespace gomoku
{
	Scene::Scene()
	{
		mButtons.reserve(static_cast<int>(eButton::Count));
	}

	void Scene::ShowButtons() const
	{
		for (auto it = mButtons.begin(); it != mButtons.end(); ++it)
		{
			ShowWindow(*it, SW_SHOW);
		}
	}

	void Scene::HideButtons() const
	{
		for (auto it = mButtons.begin(); it != mButtons.end(); ++it)
		{
			ShowWindow(*it, SW_HIDE);
		}
	}
	void Scene::redrawButtons() const
	{
		for (auto it = mButtons.begin(); it != mButtons.end(); ++it)
		{
			InvalidateRect(*it, nullptr, false);
			UpdateWindow(*it);
		}
	}
}