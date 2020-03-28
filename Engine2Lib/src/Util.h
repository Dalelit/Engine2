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

	class FileWatcher
	{
	public:
		FileWatcher(std::string filename) : filename(filename) { Check(); }

		inline std::string& GetFilename() { return filename; }

		bool Check() {
			auto currentWriteTime = std::filesystem::last_write_time(filename);
			if (currentWriteTime == lastWriteTime) return false;
			lastWriteTime = currentWriteTime;
			return true;
		}

	protected:
		std::string filename;
		std::filesystem::file_time_type lastWriteTime;
	};
}