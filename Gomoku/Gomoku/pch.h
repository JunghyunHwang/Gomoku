#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <cstdint>
#include <vector>
#include <iostream>
#include <thread>

// Direct2D
#pragma comment(lib, "d2d1.lib")
#include <d2d1.h>
#include <d2d1helper.h>

// WSA
#pragma comment(lib, "ws2_32")
#include <winSock2.h>
#include <ws2tcpip.h>

#include "GomokuDefines.h"
#include "Macro.h"

#ifdef _DEBUG
#define ASSERT(expr)	if (!(expr)) { __debugbreak(); }
#else
#define ASSERT(expr)
#endif

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != nullptr)
	{
		(*ppInterfaceToRelease)->Release();
		*ppInterfaceToRelease = nullptr;
	}
}