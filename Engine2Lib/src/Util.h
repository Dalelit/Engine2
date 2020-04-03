#pragma once
#include "pch.h"

namespace Engine2
{
	class Util
	{
	public:
		static std::wstring ToWString(std::string str);
		static std::string  ToString(std::wstring wstr);

		class Random
		{
		public:
			Random() : gen(rd()), dist(0.0f, 1.0f) {}
			Random(float min, float max) : gen(rd()), dist(min, max) {}
			float Next() { return dist(gen); }

		protected:
			std::random_device rd;
			std::mt19937_64 gen;
			std::uniform_real_distribution<float> dist;
		};
	};

	class FileWatcher
	{
	public:
		FileWatcher(std::string filename) : filename(filename) { Check(); }

		inline std::string& GetFilename() { return filename; }

		// note: replace std::filesystem::last_write_time(filename) with below as it was doing memory allocations
		bool Check() {
			WIN32_FILE_ATTRIBUTE_DATA data;
			bool result = GetFileAttributesExA(filename.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
			if (lastWriteTime.dwLowDateTime != data.ftLastWriteTime.dwLowDateTime || lastWriteTime.dwHighDateTime != data.ftLastWriteTime.dwHighDateTime)
			{
				lastWriteTime = data.ftLastWriteTime;
				return true; // has changed
			}
			return false; // no change
		}

	protected:
		std::string filename;
		FILETIME lastWriteTime = {};
	};
}