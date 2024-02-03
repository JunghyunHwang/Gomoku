#pragma once
#include "pch.h"
#include "GomokuHelper.h"

namespace gomoku
{
	enum class eStoneColor;

	class GameManager final
	{
	public:
		static GameManager* GetInstance();

		void AddStone();
		void SetGuideStonePosition(uint32_t x, uint32_t y);
		bool IsValidePosition() const;

		inline const std::vector<std::vector<eStoneColor>>& GetBoard() const;
		inline eStoneColor GetCurrentStoneColor() const;
		inline POINT GetGuideStonePosition() const;
		inline bool IsGameOver() const;

	private:
		GameManager();
		~GameManager();
		GameManager(const GameManager* other) = delete;
		GameManager& operator=(const GameManager* rhs) = delete;

		bool checkGameOverRecursive(uint32_t x, uint32_t y, uint32_t chainingCount);
		inline void switchTurn();

	private:
		static GameManager* mInstance;
		std::vector<std::vector<eStoneColor>> mBoard;
		eStoneColor mCurrentStoneColor;
		POINT mGuideStonePosition;
		bool mbGameOver;
	};

	void GameManager::switchTurn()
	{
		int curr = static_cast<int>(mCurrentStoneColor);
		mCurrentStoneColor = static_cast<eStoneColor>(curr ^ CURR_STONE_CHANGE);
	}

	const std::vector<std::vector<eStoneColor>>& GameManager::GetBoard() const
	{
		return mBoard;
	}

	eStoneColor GameManager::GetCurrentStoneColor() const
	{
		return mCurrentStoneColor;
	}

	POINT GameManager::GetGuideStonePosition() const
	{
		return mGuideStonePosition;
	}

	bool GameManager::IsGameOver() const
	{
		return mbGameOver;
	}
}
