#pragma once
#include "pch.h"

namespace Engine2
{
	class Util
	{
	public:
		static std::wstring ToWString(std::string str);
		static std::string  ToString(std::wstring wstr);
	};
}