#pragma once
#include "pch.h"

namespace Engine2
{
	namespace Util
	{
		std::wstring ToWString(const std::string& str);
		std::string  ToString(const std::wstring& wstr);

		inline bool FileExists(const std::string& filename) { return std::filesystem::exists(filename); }

		std::string CurrentDirectory();

		// check if the 'value' ends with the 'ending'
		inline bool StringEndsWith(const std::string& value, const std::string& ending)
		{
			if (value.size() < ending.size()) return false;
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}

		template <typename T>
		std::string TypeNameClean()
		{
			std::string_view view(typeid(T).name());
			return std::string(view.substr(view.find_first_of(' ') + 1));
		}

		template <typename T>
		std::string TypeNameClass()
		{
			std::string_view view(typeid(T).name());
			return std::string(view.substr(view.find_last_of(':') + 1));
		}

		class Random
		{
		public:
			Random() : gen(rd()), dist(0.0f, 1.0f) {}
			Random(float min, float max) : gen(rd()), dist(min, max) {}
			Random(Random& r) : gen(rd()), dist(r.dist) {}
			Random(Random&& r) : gen(rd()), dist(r.dist) {}

			inline float Next() { return dist(gen); }
			inline DirectX::XMVECTOR NextXMVECTORXYZ0() { return { Next(), Next(), Next(), 0.0f }; }
			inline DirectX::XMVECTOR NextXMVECTORXYZ1() { return { Next(), Next(), Next(), 1.0f }; }
			inline DirectX::XMVECTOR NextXMVECTORXYZW() { return { Next(), Next(), Next(), Next() }; }

		protected:
			std::random_device rd;
			std::mt19937_64 gen;
			std::uniform_real_distribution<float> dist;
		};

		static Random rng;

		DirectX::XMVECTOR RandomOnUnitSphere();
		DirectX::XMVECTOR RandomInUnitSphere();
	};
}