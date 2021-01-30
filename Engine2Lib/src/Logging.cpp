#include "pch.h"
#include "Logging.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// To Do: actually thing about this and do it properly

namespace Engine2
{
	void Logging::LogInfo(const char* msg)
	{
		OutputDebugStringA(msg);
		OutputDebugStringA("\n");
	}

	void Logging::LogWarning(const char* msg)
	{
		OutputDebugStringA(msg);
		OutputDebugStringA("\n");
	}

	void Logging::LogError(const char* msg)
	{
		OutputDebugStringA(msg);
		OutputDebugStringA("\n");
	}
}