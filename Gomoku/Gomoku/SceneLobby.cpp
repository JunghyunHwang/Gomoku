#include "pch.h"
#include "SceneLobby.h"

namespace gomoku
{
	SceneLobby::SceneLobby()
		: Scene()
	{
	}

	void SceneLobby::Render()
	{
		RENDER_TARGET* renderTarget = mSceneManager->getRenderTaget();
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
