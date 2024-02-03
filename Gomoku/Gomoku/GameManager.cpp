#include "pch.h"
#include "GameManager.h"
#include "eStoneType.h"

namespace gomoku
{
	GameManager* GameManager::mInstance = nullptr;

	GameManager* GameManager::GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new GameManager();
		}

		return mInstance;
	}

	GameManager::GameManager()
		: mCurrentStoneColor(eStoneColor::Black)
		, mGuideStonePosition({ NONE, NONE })
		, mbGameOver(false)
	{
		for (int y = 0; y < LINE_COUNT; ++y)
		{
			std::vector<eStoneColor> temp;
			temp.reserve(LINE_COUNT);

			for (int x = 0; x < LINE_COUNT; ++x)
			{
				temp.push_back(eStoneColor::None);
			}

			mBoard.push_back(temp);
		}
	}

	GameManager::~GameManager()
	{
		delete mInstance;
	}

	void GameManager::AddStone()
	{
		if (!IsValidePosition())
		{
			return;
		}

		mBoard[mGuideStonePosition.y][mGuideStonePosition.x] = mCurrentStoneColor;
		checkGameOverRecursive(mGuideStonePosition.x, mGuideStonePosition.y, 0);

		switchTurn();
	}

	void GameManager::SetGuideStonePosition(uint32_t x, uint32_t y)
	{
		ASSERT(LINE_INTERVAL < 256);
		const int32_t NOMALIZED_X = (x - BOARD_START_POINT);
		const int32_t NOMALIZED_Y = (y - BOARD_START_POINT);

		int8_t col = NOMALIZED_X / LINE_INTERVAL;
		int8_t row = NOMALIZED_Y / LINE_INTERVAL;

		int8_t colAlpha = NOMALIZED_X % LINE_INTERVAL;
		int8_t rowAlpha = NOMALIZED_Y % LINE_INTERVAL;

		colAlpha -= static_cast<int8_t>(HALF_LINE_INTERVAL);
		colAlpha >>= 7;
		colAlpha += 1;

		rowAlpha -= static_cast<int8_t>(HALF_LINE_INTERVAL);
		rowAlpha >>= 7;
		rowAlpha += 1;

		col += colAlpha;
		row += rowAlpha;

		if (col < 0 || col >= LINE_COUNT
			|| row < 0 || row >= LINE_COUNT)
		{
			col = NONE;
			row = NONE;
		}

		mGuideStonePosition = { col, row };
	}

	bool GameManager::IsValidePosition() const
	{
		if (mGuideStonePosition.x != NONE && mGuideStonePosition.y != NONE
			&& mBoard[mGuideStonePosition.y][mGuideStonePosition.x] == eStoneColor::None)
		{
			return true;
		}

		return false;
	}

	bool GameManager::checkGameOverRecursive(uint32_t x, uint32_t y, uint32_t chainingCount)
	{
		if (x < 0 || x == LINE_COUNT
			|| y < 0 || y == LINE_COUNT
			|| mBoard[y][x] == mCurrentStoneColor)
		{
			return false;
		}
		else if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}
	}
}