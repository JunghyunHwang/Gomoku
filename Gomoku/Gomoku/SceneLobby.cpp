#include "pch.h"
#include "SceneLobby.h"
#include "GameManager.h"

namespace gomoku
{
	void SceneLobby::render()
	{
		RENDER_TARGET* renderTarget = SceneManager::getRenderTaget();
		ASSERT(renderTarget != nullptr);

		renderTarget->BeginDraw();
		{
			renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			renderTarget->Clear(D2D1::ColorF(BOARD_COLOR));
		}
		renderTarget->EndDraw();

		redrawButtons();
	}
}
