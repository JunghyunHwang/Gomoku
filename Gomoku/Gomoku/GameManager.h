#pragma once
#include "pch.h"
#include "GomokuHelper.h"
#include "eStoneColor.h"

namespace gomoku
{
	enum class eStoneColor;
	class Socket;

	class GameManager final
	{
	public:
		static GameManager* GetInstance();

		void SetNewGame();
		void AddStone();
		void SetGuideStonePosition(uint32_t x, uint32_t y);
		int Bind();
		int FindOppnent(_Out_ SOCKADDR_IN& oppnentAddr);
		int ConnectOppnent(SOCKADDR_IN& oppnentAddr);

		inline bool IsValidePosition() const;
		inline const std::vector<std::vector<eStoneColor>>& GetBoard() const;
		inline eStoneColor GetCurrentStoneColor() const;
		inline POINT GetGuideStonePosition() const;
		inline bool IsGameOver() const;
		inline eStoneColor GetWinnerStone() const;

	private:
		GameManager();
		~GameManager();
		GameManager(const GameManager* other) = delete;
		GameManager& operator=(const GameManager* rhs) = delete;

		/* Check gomoku*/
		bool checkGameOver() const;
		bool checkHorizontal() const;
		uint32_t checkWestRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkEastRecursive(uint32_t x, uint32_t y) const;

		bool checkVertical() const;
		uint32_t checkNorthRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkSouthRecursive(uint32_t x, uint32_t y) const;

		bool checkLeftDiagonal() const;
		uint32_t checkNorthWestRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkSouthEastRecursive(uint32_t x, uint32_t y) const;

		bool checkRightDiagonal() const;
		uint32_t checkNorthEastRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkSouthWestRecursive(uint32_t x, uint32_t y) const;

		void recvFromOppnent();
		inline void switchTurn();
		
	private:
		static GameManager* mInstance;

		std::vector<std::vector<eStoneColor>> mBoard;
		eStoneColor mCurrentStoneColor;
		eStoneColor mWinnerStoneColor;
		POINT mGuideStonePosition;
		bool mbGameOver;

		enum
		{
			HOST,
			GUEST,
			BUFFER_SIZE = 128,
			MYSOCKET_PORT = 20000,
		};

		SOCKET mMySocket;
		SOCKET mServerSocket;
		SOCKET mOppnentSocket;
		std::thread mRecvThread;
		char mBuffer[BUFFER_SIZE] = { 0, };
	};

	bool GameManager::IsValidePosition() const
	{
		return (mGuideStonePosition.x != NONE && mGuideStonePosition.y != NONE
			&& mBoard[mGuideStonePosition.y][mGuideStonePosition.x] == eStoneColor::None);
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

	eStoneColor GameManager::GetWinnerStone() const
	{
		return mWinnerStoneColor;
	}

	void GameManager::switchTurn()
	{
		int curr = static_cast<int>(mCurrentStoneColor);
		mCurrentStoneColor = static_cast<eStoneColor>(curr ^ CURR_STONE_CHANGE);
	}
}
