#pragma once
#include "eStoneColor.h"
#include "SceneManager.h"

namespace gomoku
{
	class App;
	class GameManager final
	{
		friend App;
	public:
		static void SetNewGame();
		static void AddStone();
		static void SetGuideStonePosition(uint32_t x, uint32_t y);
		static void InitGame();

		static int BindAndListen();
		static int ConnectOpponent();

		static inline bool IsValidePosition();
		static inline const std::vector<std::vector<eStoneColor>>& GetBoard();
		static inline bool IsMyTurn();
		static inline eStoneColor GetStoneColor();
		static inline eStoneColor GetCurrentStoneColor();
		static inline const POINT& GetGuideStonePosition();
		static inline bool IsGameOver();
		static inline eStoneColor GetWinnerStone();

	private:
		GameManager() = delete;
		~GameManager() = delete;
		GameManager(const GameManager* other) = delete;
		GameManager& operator=(const GameManager* rhs) = delete;

	private:
		static HRESULT init();
		static void release();

		static inline void switchTurn();
		static void acceptOppnent();
		static void recvFromServer();
		static void recvFromOpponent(SOCKET recvSock);

		/* Check gomoku*/
		static bool checkGameOver(POINT& p);
		static bool checkHorizontal(POINT& p);
		static uint32_t checkWestRecursive(uint32_t x, uint32_t y);
		static uint32_t checkEastRecursive(uint32_t x, uint32_t y);

		static bool checkVertical(POINT& p);
		static uint32_t checkNorthRecursive(uint32_t x, uint32_t y);
		static uint32_t checkSouthRecursive(uint32_t x, uint32_t y);

		static bool checkLeftDiagonal(POINT& p);
		static uint32_t checkNorthWestRecursive(uint32_t x, uint32_t y);
		static uint32_t checkSouthEastRecursive(uint32_t x, uint32_t y);

		static bool checkRightDiagonal(POINT& p);
		static uint32_t checkNorthEastRecursive(uint32_t x, uint32_t y);
		static uint32_t checkSouthWestRecursive(uint32_t x, uint32_t y);

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

		static BOARD mBoard;
		static SOCKET mListenSocket;
		static SOCKET mServerSocket;
		static SOCKET mSendSocket;

		static bool mbGameOver;
		static bool mbIsMyTurn;
		static eStoneColor mMyStone;
		static eStoneColor mCurrentTurnStone;
		static POINT mGuideStonePosition;

		static char mBuffer[BUFFER_SIZE];

		static std::thread mServerRecvThread;
		static std::thread mAcceptThread;
		static std::thread mRecvThread;
	};

	bool GameManager::IsValidePosition()
	{
		return (mGuideStonePosition.x != NONE && mGuideStonePosition.y != NONE
			&& mBoard[mGuideStonePosition.y][mGuideStonePosition.x] == eStoneColor::None);
	}

	const std::vector<std::vector<eStoneColor>>& GameManager::GetBoard() 
	{
		return mBoard;
	}

	bool GameManager::IsMyTurn() 
	{
		return mbIsMyTurn;
	}

	eStoneColor GameManager::GetStoneColor() 
	{
		return mMyStone;
	}

	eStoneColor GameManager::GetCurrentStoneColor()
	{
		return mCurrentTurnStone;
	}

	const POINT& GameManager::GetGuideStonePosition() 
	{
		return mGuideStonePosition;
	}

	bool GameManager::IsGameOver() 
	{
		return mbGameOver;
	}

	eStoneColor GameManager::GetWinnerStone() 
	{
		return mbGameOver ? mCurrentTurnStone : eStoneColor::None;
	}

	void GameManager::switchTurn()
	{
		mCurrentTurnStone = static_cast<eStoneColor>(ENUM_CAST_INT(mCurrentTurnStone) ^ XOR_CHANGE_TURN);

		if (SceneManager::GetCurrentScene() == eScene::Matching)
		{
			mbIsMyTurn ^= XOR_CHANGE_TURN;
		}
	}
}
