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