#pragma once

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

// Direct2D
#pragma comment(lib, "d2d1.lib")
#include <d2d1.h>
#include <d2d1helper.h>

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(expr) if (!(expr)) { __debugbreak(); }
#else
#define ASSERT(expr)
#endif
#endif /* ASSERT */

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != nullptr)
	{
		(*ppInterfaceToRelease)->Release();
		*ppInterfaceToRelease = nullptr;
	}
}