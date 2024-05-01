#include "pch.h"
#include "App.h"
#include "GameManager.h"

namespace gomoku
{
	/*
		Todo
		* When socket create fail
		* Closing window during a game
	*/

	BOARD GameManager::mBoard(LINE_COUNT, std::vector<eStoneColor>(LINE_COUNT, eStoneColor::None));
	SOCKET GameManager::mListenSocket = INVALID_SOCKET;
	SOCKET GameManager::mServerSocket = INVALID_SOCKET;
	SOCKET GameManager::mSendSocket = INVALID_SOCKET;

	bool GameManager::mbGameOver = true;
	bool GameManager::mbIsMyTurn = false;
	eStoneColor GameManager::mMyStone = eStoneColor::None;
	eStoneColor GameManager::mCurrentTurnStone = eStoneColor::None;
	POINT GameManager::mGuideStonePosition = { NONE, NONE };

	char GameManager::mBuffer[BUFFER_SIZE] = { 0, };

	std::thread GameManager::mServerRecvThread;
	std::thread GameManager::mAcceptThread;
	std::thread GameManager::mRecvThread;

	HRESULT GameManager::init()
	{
		HRESULT hr = S_OK;

		mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mListenSocket == INVALID_SOCKET)
		{
			hr = E_FAIL;
			goto FAIL_RETURN;
		}

		mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mServerSocket == INVALID_SOCKET)
		{
			closesocket(mListenSocket);

			hr = E_FAIL;
			goto FAIL_RETURN;
		}

		mSendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mSendSocket == INVALID_SOCKET)
		{
			closesocket(mListenSocket);
			closesocket(mServerSocket);
			
			hr = E_FAIL;
		}

	FAIL_RETURN:
		return hr;
	}

	void GameManager::release()
	{
		closesocket(mListenSocket);
		closesocket(mServerSocket);
		closesocket(mSendSocket);

		mListenSocket = INVALID_SOCKET;
		mServerSocket = INVALID_SOCKET;
		mSendSocket = INVALID_SOCKET;

		if (mServerRecvThread.joinable())
		{
			mServerRecvThread.join();
		}

		if (mAcceptThread.joinable())
		{
			mAcceptThread.join();
		}

		if (mRecvThread.joinable())
		{
			mRecvThread.join();
		}
	}

	int GameManager::BindAndListen()
	{
		// Get ip address by host name
		wchar_t hostName[COMPUTER_NAME_SIZE] = { 0, };
		char ipStr[IP_STR_SIZE] = { 0, };
		ADDRINFOW* hostAddrInfo = NULL;
		ADDRINFOW hints;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Need error handle
		GetHostNameW(hostName, COMPUTER_NAME_SIZE);
		GetAddrInfoW(hostName, NULL, &hints, &hostAddrInfo);

		sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(hostAddrInfo->ai_addr);
		inet_ntop(AF_INET, &addr->sin_addr, ipStr, IP_STR_SIZE);
		FreeAddrInfoW(hostAddrInfo);

		// Bind
		SOCKADDR_IN myAddr = { 0, };
		myAddr.sin_family = AF_INET;
		myAddr.sin_port = htons(LISTEN_PORT);
		inet_pton(AF_INET, ipStr, &myAddr.sin_addr);

		if (bind(mListenSocket, (SOCKADDR*)&myAddr, sizeof(myAddr)) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		if (listen(mListenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		mAcceptThread = std::thread(&GameManager::acceptOppnent);
		return S_OK;
	}

	void GameManager::acceptOppnent()
	{
		SOCKET recvSock;
		SOCKADDR_IN client = { 0, };
		int clientAddrSize = sizeof(client);

		while ((recvSock = accept(mListenSocket, (SOCKADDR*)&client, &clientAddrSize)) != INVALID_SOCKET)
		{
			mRecvThread = std::thread(&GameManager::recvFromOpponent, recvSock);
		}
	}

	void GameManager::recvFromOpponent(SOCKET recvSock)
	{
		POINT p;
		while (recv(recvSock, (char*)&p, sizeof(p), 0) > 0)
		{
			ASSERT(!mbIsMyTurn);
			ASSERT(p.x >= 0 && p.x < LINE_COUNT);
			ASSERT(p.y >= 0 && p.y < LINE_COUNT);

			mBoard[p.y][p.x] = mCurrentTurnStone;

			App::render();

			if (checkGameOver(p))
			{
				App::notifyWinner(mCurrentTurnStone);
				SetNewGame();
			}
		}

		closesocket(recvSock);
		closesocket(mSendSocket);
	}

	int GameManager::ConnectOpponent()
	{
		SOCKADDR_IN serverAddr = { 0, };
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, "192.168.55.6", &serverAddr.sin_addr); // My server address

		if (connect(mServerSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			return WSAGetLastError();
		}

		mServerRecvThread = std::thread(&GameManager::recvFromServer);
		return S_OK;
	}

	void GameManager::recvFromServer()
	{
		SOCKADDR_IN oppnentAddr = { 0, };
		oppnentAddr.sin_family = AF_INET;
		oppnentAddr.sin_port = htons(OPPNENT_PORT);

		recv(mServerSocket, mBuffer, BUFFER_SIZE, 0); // Receive Oppnent address
		closesocket(mServerSocket);

		char* tokenized = strtok(mBuffer, DELIM);
		ASSERT(tokenized != NULL);
		mMyStone = static_cast<eStoneColor>(atoi(tokenized));

		tokenized = strtok(NULL, DELIM);
		ASSERT(tokenized != NULL);
		oppnentAddr.sin_addr.S_un.S_addr = atoi(tokenized);

		ASSERT(strtok(NULL, DELIM) == NULL);

		if (connect(mSendSocket, (SOCKADDR*)&oppnentAddr, sizeof(oppnentAddr)) == SOCKET_ERROR)
		{
			std::cout << "Faild to connect opponent with error code: " << WSAGetLastError() << std::endl;
			return;
		}

		SetNewGame();
	}

	void GameManager::SetNewGame()
	{
		ASSERT(mBoard.size() == LINE_COUNT);
		ASSERT(mBoard[0].size() == LINE_COUNT);
		ASSERT(mbGameOver);

		mCurrentTurnStone = eStoneColor::Black;

		if (mMyStone == eStoneColor::Black
			|| SceneManager::GetCurrentScene() == eScene::Practice)
		{
			mbIsMyTurn = true;
		}

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

		// if cursor goes over half line interval.
		int8_t colAlpha = NOMALIZED_X % LINE_INTERVAL;
		int8_t rowAlpha = NOMALIZED_Y % LINE_INTERVAL;
		colAlpha -= static_cast<int8_t>(HALF_LINE_INTERVAL);
		colAlpha >>= 7;
		colAlpha += 1;
		rowAlpha -= static_cast<int8_t>(HALF_LINE_INTERVAL);
		rowAlpha >>= 7;
		rowAlpha += 1;

		// Alpha value always 0 or 1
		ASSERT(colAlpha == 0 || colAlpha == 1);
		ASSERT(rowAlpha == 0 || rowAlpha == 1);
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

	void GameManager::InitGame()
	{
		for (size_t y = 0; y < LINE_COUNT; ++y)
		{
			for (size_t x = 0; x < LINE_COUNT; ++x)
			{
				mBoard[y][x] = eStoneColor::None;
			}
		}

		mGuideStonePosition.x = NONE;
		mGuideStonePosition.y = NONE;
		mMyStone = eStoneColor::None;
		mbIsMyTurn = false;
		mbGameOver = true;
	}

	void GameManager::AddStone()
	{
		ASSERT(!mbGameOver);
		
		if (!mbIsMyTurn || !IsValidePosition())
		{
			return;
		}

		mBoard[mGuideStonePosition.y][mGuideStonePosition.x] = mCurrentTurnStone;
		POINT p = { mGuideStonePosition.x, mGuideStonePosition.y };

		if (SceneManager::GetCurrentScene() == eScene::Matching)
		{
			send(mSendSocket, (const char*)&p, sizeof(p), 0);
		}

		checkGameOver(p);
	}

	/* Check gomoku*/
	bool GameManager::checkGameOver(POINT& p)
	{
		if (checkHorizontal(p)
			|| checkVertical(p)
			|| checkLeftDiagonal(p)
			|| checkRightDiagonal(p))
		{
			InitGame();

			return true;
		}

		switchTurn();
		return false;
	}

	bool GameManager::checkHorizontal(POINT& p) 
	{
		uint32_t chainingCount = 1;
		chainingCount += checkWestRecursive(p.x - 1, p.y);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkEastRecursive(p.x + 1, p.y);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkWestRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkWestRecursive(x - 1, y) + 1;
	}

	uint32_t GameManager::checkEastRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkEastRecursive(x + 1, y) + 1;
	}

	bool GameManager::checkVertical(POINT& p) 
	{
		uint32_t chainingCount = 1;
		chainingCount += checkNorthRecursive(p.x, p.y - 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkSouthRecursive(p.x, p.y + 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkNorthRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkNorthRecursive(x, y - 1) + 1;
	}

	uint32_t GameManager::checkSouthRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkSouthRecursive(x, y + 1) + 1;
	}

	bool GameManager::checkLeftDiagonal(POINT& p) 
	{
		uint32_t chainingCount = 1;
		chainingCount += checkNorthWestRecursive(p.x - 1, p.y - 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkSouthEastRecursive(p.x + 1, p.y + 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkNorthWestRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkNorthWestRecursive(x - 1, y - 1) + 1;
	}

	uint32_t GameManager::checkSouthEastRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkSouthEastRecursive(x + 1, y + 1) + 1;
	}

	bool GameManager::checkRightDiagonal(POINT& p) 
	{
		uint32_t chainingCount = 1;
		chainingCount += checkNorthEastRecursive(p.x + 1, p.y - 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		chainingCount += checkSouthWestRecursive(p.x - 1, p.y + 1);

		if (chainingCount == MAX_CHAINING_COUNT)
		{
			return true;
		}

		return false;
	}

	uint32_t GameManager::checkNorthEastRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkNorthEastRecursive(x + 1, y - 1) + 1;
	}

	uint32_t GameManager::checkSouthWestRecursive(uint32_t x, uint32_t y) 
	{
		if (x < 0 || x >= LINE_COUNT
			|| y < 0 || y >= LINE_COUNT
			|| mBoard[y][x] != mCurrentTurnStone)
		{
			return 0;
		}

		return checkSouthWestRecursive(x - 1, y + 1) + 1;
	}
}