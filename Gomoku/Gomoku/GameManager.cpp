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
		, mbGameOver(true)
		, mMySocket(socket(AF_INET, SOCK_STREAM, 0))
		, mServerSocket(socket(AF_INET, SOCK_STREAM, 0))
		, mOppnentSocket(socket(AF_INET, SOCK_STREAM, 0))
		, mBoard(LINE_COUNT, std::vector<eStoneColor>(LINE_COUNT, eStoneColor::None))
	{
		// When fail to create socket
	}

	GameManager::~GameManager()
	{
		closesocket(mMySocket);
		closesocket(mServerSocket);
		closesocket(mOppnentSocket);

		delete mInstance;
	}

	int GameManager::Bind()
	{
		SOCKADDR_IN myAddr = { 0, };
		myAddr.sin_family = AF_INET;
		myAddr.sin_port = htons(MYSOCKET_PORT);
		inet_pton(AF_INET, "192.168.55.6", &myAddr.sin_addr);

		if (bind(mMySocket, (SOCKADDR*)&myAddr, sizeof(myAddr)) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		if (listen(mMySocket, SOMAXCONN) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		return S_OK;
	}

	int GameManager::FindOppnent(_Out_ SOCKADDR_IN& oppnentAddr)
	{
		int oppnentAddrSize = sizeof(oppnentAddr);

		oppnentAddr.sin_family = AF_INET;
		oppnentAddr.sin_port = htons(SERVER_PORT_NUM);
		inet_pton(AF_INET, "192.168.55.6", &oppnentAddr.sin_addr);

		if (connect(mServerSocket, (SOCKADDR*)&oppnentAddr, oppnentAddrSize) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}
		
		memset(&oppnentAddr, 0, oppnentAddrSize);
		recv(mServerSocket, mBuffer, BUFFER_SIZE, 0); // Receive Oppnent address

		char* tokenized = strtok(mBuffer, DELIM);
		ASSERT(tokenized != NULL);

		if (atoi(tokenized) == HOST)
		{
			int ret = Bind();
			if (ret != S_OK)
			{
				return ret;
			}

			ASSERT(strtok(NULL, DELIM) == NULL);
			mOppnentSocket = accept(mMySocket, (SOCKADDR*)&oppnentAddr, &oppnentAddrSize);

			if (mOppnentSocket == INVALID_SOCKET)
			{
				return WSAGetLastError();
			}
			
			return S_OK;
		}

		oppnentAddr.sin_family = AF_INET;
		oppnentAddr.sin_port = htons(MYSOCKET_PORT);

		tokenized = strtok(NULL, DELIM);
		ASSERT(tokenized != NULL);
		oppnentAddr.sin_addr.S_un.S_addr = atoi(tokenized);

		ASSERT(strtok(NULL, DELIM) == NULL);

		if (connect(mOppnentSocket, (SOCKADDR*)&oppnentAddr, oppnentAddrSize) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		return S_OK;
	}

	int GameManager::ConnectOppnent(SOCKADDR_IN& oppnentAddr)
	{
		if (connect(mOppnentSocket, (SOCKADDR*)&oppnentAddr, sizeof(oppnentAddr)) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		// mRecvThread = std::thread(&GameManager::recvFromOppnent, this);

		return S_OK;
	}

	void GameManager::recvFromOppnent()
	{
		while (recv(mOppnentSocket, mBuffer, BUFFER_SIZE, 0) > 0)
		{
			
		}
	}

	void GameManager::SetNewGame()
	{
		ASSERT(mBoard.size() == LINE_COUNT);
		ASSERT(mBoard[0].size() == LINE_COUNT);

		if (!mbGameOver)
		{
			ASSERT(false);
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