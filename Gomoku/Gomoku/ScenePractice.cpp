#include "pch.h"
#include "ScenePractice.h"
#include "GameManager.h"

namespace gomoku
{
	GameManager* ScenePractice::mGameManager = GameManager::GetInstance();

	void ScenePractice::Render()
	{
		redrawButtons();
	}
}