#ifndef INCLUDED_COMMON_H
#define INCLUDED_COMMON_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#if defined(_CONSOLE)
	#define LOGPRINT printf
#else
	#define LOGPRINT OutputDebugStringA
#endif

#if defined(_DEBUG)
	#define LOG(format, ...)\
		{\
			char buffer[4096];\
			int ret = _snprintf_s(buffer, _countof(buffer), _TRUNCATE, (#format), __VA_ARGS__);\
			LOGPRINT(buffer);\
			if(ret == -1)\
			{\
				LOGPRINT("** message truncated **\n");\
			}\
			LOGPRINT("\n");\
		}
#else
	#define LOG(format, ...)
#endif

#if defined(_DEBUG)
	#define ASSERT(condition, format, ...)\
		{\
			if(!(condition))\
			{\
				LOG(format, __VA_ARGS__)\
				DebugBreak();\
			}\
		}
	#define VERIFY(condition, format, ...)\
		{\
			if(!(condition))\
			{\
				LOG(format, __VA_ARGS__)\
				DebugBreak();\
			}\
		}
#else
	#define ASSERT(condition, format, ...)
	#define VERIFY(condition, format, ...) condition;
#endif

template <typename T>
inline void SafeDelete(T*& ptr)
{
	delete ptr;
	ptr = nullptr;
}

template <typename T>
inline void SafeDeleteArray(T*& ptr)
{
	delete[] ptr;
	ptr = nullptr;
}

#endif // !INCLUDED_COMMON_H
