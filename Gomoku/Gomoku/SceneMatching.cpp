#include "pch.h"
#include "SceneMatching.h"
#include "eStoneColor.h"
#include "GameManager.h"

namespace gomoku
{
	void SceneMatching::render()
	{
		RENDER_TARGET* renderTarget = SceneManager::getRenderTaget();
		SOLID_BRUSH** brushes = SceneManager::getBrushes();
		ASSERT(renderTarget != nullptr);

		renderTarget->BeginDraw();
		{
			renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			renderTarget->Clear(D2D1::ColorF(BOARD_COLOR));

			SOLID_BRUSH* blackBrush = brushes[ENUM_CAST_INT(eStoneColor::Black)];
			SOLID_BRUSH* lowOpacityBlackBrush = brushes[ENUM_CAST_INT(eStoneColor::Black) + LOW_OPACITY];

			// Draw vertical line
			for (size_t x = 0; x < LINE_COUNT; ++x)
			{
				renderTarget->DrawLine(
					D2D1::Point2F(static_cast<float>(XPOS(x)), BOARD_START_POINT),
					D2D1::Point2F(static_cast<float>(XPOS(x)), YPOS(LINE_COUNT - 1)),
					blackBrush,
					static_cast<float>(LINE_WIDTH)
				);
			}

			// Draw horizontal line
			for (size_t y = 0; y < LINE_COUNT; ++y)
			{
				renderTarget->DrawLine(
					D2D1::Point2F(BOARD_START_POINT, static_cast<float>(YPOS(y))),
					D2D1::Point2F(XPOS(LINE_COUNT - 1), static_cast<float>(YPOS(y))),
					blackBrush,
					static_cast<float>(LINE_WIDTH)
				);
			}

			const BOARD& board = GameManager::GetBoard();

			// Draw guide stone
			if (GameManager::IsValidePosition() && GameManager::IsMyTurn())
			{
				eStoneColor stoneColor = GameManager::GetCurrentStoneColor();
				const POINT& currPos = GameManager::GetGuideStonePosition();

				D2D1_ELLIPSE guideStoneInfo = D2D1::Ellipse(
					D2D1::Point2F(
						static_cast<float>(LINE_INTERVAL * currPos.x + BOARD_START_POINT),
						static_cast<float>(LINE_INTERVAL * currPos.y + BOARD_START_POINT)
					),
					STONE_RADIUS,
					STONE_RADIUS
				);

				renderTarget->DrawEllipse(guideStoneInfo, lowOpacityBlackBrush, STONE_STROKE_WIDTH);
				renderTarget->FillEllipse(guideStoneInfo, brushes[ENUM_CAST_INT(stoneColor) + LOW_OPACITY]);
			}

			// Draw stone
			for (size_t y = 0; y < LINE_COUNT; ++y)
			{
				for (size_t x = 0; x < LINE_COUNT; ++x)
				{
					if (board[y][x] == eStoneColor::None)
					{
						continue;
					}

					D2D1_ELLIPSE stoneInfo = D2D1::Ellipse(
						D2D1::Point2F(
							static_cast<float>(LINE_INTERVAL * x + BOARD_START_POINT),
							static_cast<float>(LINE_INTERVAL * y + BOARD_START_POINT)
						),
						STONE_RADIUS,
						STONE_RADIUS
					);

					renderTarget->DrawEllipse(stoneInfo, blackBrush, STONE_STROKE_WIDTH);
					renderTarget->FillEllipse(stoneInfo, brushes[ENUM_CAST_INT(board[y][x])]);
				}
			}
		}
		renderTarget->EndDraw();
	}
}