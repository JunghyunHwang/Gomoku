#pragma once
#include "eStoneColor.h"

namespace gomoku
{
	class App;
	class GameManager final
	{
		friend App;
	public:
		static GameManager* GetInstance();

		void SetNewGame();
		void AddStone();
		void SetGuideStonePosition(uint32_t x, uint32_t y);

		int BindAndListen();
		int ConnectOpponent();

		inline bool IsValidePosition() const;
		inline const std::vector<std::vector<eStoneColor>>& GetBoard() const;
		inline bool IsMyTurn() const;
		inline eStoneColor GetStoneColor() const;
		inline const POINT& GetGuideStonePosition() const;
		inline bool IsGameOver() const;
		inline eStoneColor GetWinnerStone() const;

	private:
		GameManager();
		~GameManager() = default;
		GameManager(const GameManager* other) = delete;
		GameManager& operator=(const GameManager* rhs) = delete;

		inline void switchTurn();
		void acceptOppnent();
		void recvFromServer();
		void recvFromOpponent(SOCKET recvSock);
		void release();

		/* Check gomoku*/
		bool checkGameOver(POINT& p);
		bool checkHorizontal(POINT& p) const;
		uint32_t checkWestRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkEastRecursive(uint32_t x, uint32_t y) const;

		bool checkVertical(POINT& p) const;
		uint32_t checkNorthRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkSouthRecursive(uint32_t x, uint32_t y) const;

		bool checkLeftDiagonal(POINT& p) const;
		uint32_t checkNorthWestRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkSouthEastRecursive(uint32_t x, uint32_t y) const;

		bool checkRightDiagonal(POINT& p) const;
		uint32_t checkNorthEastRecursive(uint32_t x, uint32_t y) const;
		uint32_t checkSouthWestRecursive(uint32_t x, uint32_t y) const;

	private:
		enum
		{
			MAX_CHAINING_COUNT = 5,
			COMPUTER_NAME_SIZE = 16,
			IP_STR_SIZE = 16,
			BUFFER_SIZE = 128,
			LISTEN_PORT = 20000,
			OPPNENT_PORT = 21000,
			SERVER_PORT = 25000,
		};

		static GameManager* mInstance;
		static BOARD mBoard;

		bool mbGameOver;
		bool mbIsMyTurn;
		eStoneColor mMyStone;
		eStoneColor mCurrentTurnStone;
		eStoneColor mWinnerStone;
		POINT mGuideStonePosition;

		SOCKET mListenSocket;
		SOCKET mServerSocket;
		SOCKET mSendSocket;

		char mBuffer[BUFFER_SIZE] = { 0, };

		std::thread mServerRecvThread;
		std::thread mAcceptThread;
		std::thread mRecvThread;
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

	bool GameManager::IsMyTurn() const
	{
		return mbIsMyTurn;
	}

	eStoneColor GameManager::GetStoneColor() const
	{
		return mMyStone;
	}

	const POINT& GameManager::GetGuideStonePosition() const
	{
		return mGuideStonePosition;
	}

	bool GameManager::IsGameOver() const
	{
		return mbGameOver;
	}

	eStoneColor GameManager::GetWinnerStone() const
	{
		return mWinnerStone;
	}

	void GameManager::switchTurn()
	{
		mCurrentTurnStone = static_cast<eStoneColor>(static_cast<int>(mCurrentTurnStone) ^ XOR_CHANGE_TURN);
		mbIsMyTurn ^= XOR_CHANGE_TURN;
	}
}
