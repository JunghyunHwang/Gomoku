#include "pch.h"
#include "GameManager.h"

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
		, mWinnerStoneColor(eStoneColor::None)
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

	void GameManager::SetNewGame()
	{
		ASSERT(mBoard.size() == LINE_COUNT);
		ASSERT(mBoard[0].size() == LINE_COUNT);

		if (!mbGameOver)
		{
			return;
		}

		for (size_t y = 0; y < LINE_COUNT; ++y)
		{
			for (size_t x = 0; x < LINE_COUNT; ++x)
			{
				mBoard[y][x] = eStoneColor::None;
			}
		}
		
		mCurrentStoneColor = eStoneColor::Black;
		mWinnerStoneColor = eStoneColor::None;
		mGuideStonePosition.x = NONE;
		mGuideStonePosition.y = NONE;
		mbGameOver = false;
	}

	void GameManager::AddStone()
	{
		ASSERT(!mbGameOver);
		if (!IsValidePosition())
		{
			return;
		}

		mBoard[mGuideStonePosition.y][mGuideStonePosition.x] = mCurrentStoneColor;
		if (checkGameOver())
		{
			mWinnerStoneColor = mCurrentStoneColor;
			mbGameOver = true;
			return;
		}

		switchTurn();
	}

	void GameManager::SetGuideStonePosition(uint32_t x, uint32_t y)
	{
		ASSERT(!mbGameOver);
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

	bool GameManager::checkGameOver() const
	{
		if (checkHorizontal() || checkVertical()
			|| checkLeftDiagonal() || checkRightDiagonal())
		{
			return true;
		}

		return false;
	}

	bool GameManager::checkHorizontal() const
	{
		uint32_t chainingCount = 1;
		chainingCount += checkWestRecursive(mGuideStonePosition.x - 1, mGuideStonePosition.y);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkEastRecursive(mGuideStonePosition.x + 1, mGuideStonePosition.y);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkWestRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkWestRecursive(x - 1, y) + 1;
	}

	uint32_t GameManager::checkEastRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkEastRecursive(x + 1, y) + 1;
	}

	bool GameManager::checkVertical() const
	{
		uint32_t chainingCount = 1;
		chainingCount += checkNorthRecursive(mGuideStonePosition.x, mGuideStonePosition.y - 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkSouthRecursive(mGuideStonePosition.x, mGuideStonePosition.y + 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkNorthRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkNorthRecursive(x, y - 1) + 1;
	}

	uint32_t GameManager::checkSouthRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkSouthRecursive(x, y + 1) + 1;
	}

	bool GameManager::checkLeftDiagonal() const
	{
		uint32_t chainingCount = 1;
		chainingCount += checkNorthWestRecursive(mGuideStonePosition.x - 1, mGuideStonePosition.y - 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkSouthEastRecursive(mGuideStonePosition.x + 1, mGuideStonePosition.y + 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkNorthWestRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkNorthWestRecursive(x - 1, y - 1) + 1;
	}

	uint32_t GameManager::checkSouthEastRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkSouthEastRecursive(x + 1, y + 1) + 1;
	}

	bool GameManager::checkRightDiagonal() const
	{
		uint32_t chainingCount = 1;
		chainingCount += checkNorthEastRecursive(mGuideStonePosition.x + 1, mGuideStonePosition.y - 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkSouthWestRecursive(mGuideStonePosition.x - 1, mGuideStonePosition.y + 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkNorthEastRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkNorthEastRecursive(x + 1, y - 1) + 1;
	}

	uint32_t GameManager::checkSouthWestRecursive(uint32_t x, uint32_t y) const
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentStoneColor)
		{
			return 0;
		}

		return checkSouthWestRecursive(x - 1, y + 1) + 1;
	}
}