#include "pch.h"
#include "Util.h"
#include "DXDevice.h"

namespace Engine2
{
	namespace Util
	{
		std::wstring ToWString(const std::string& str)
		{
			int tgtlength;
			int srclength = (int)str.length();
			tgtlength = MultiByteToWideChar(CP_ACP, 0, str.c_str(), srclength, 0, 0);
			std::wstring r(tgtlength, L'\0');
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), srclength, &r[0], tgtlength);
			return r;
		}

		std::string ToString(const std::wstring& wstr)
		{
			int tgtlength;
			int srclength = (int)wstr.length();
			tgtlength = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), srclength, 0, 0, 0, 0);
			std::string r(tgtlength, '\0');
			WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), srclength, &r[0], tgtlength, 0, 0);
			return r;
		}

		DirectX::XMVECTOR RandomOnUnitSphere()
		{
			float xzAng = DirectX::XM_2PI * rng.Next();
			float yzAng = DirectX::XM_PI * rng.Next();
			return { sinf(yzAng) * cosf(xzAng), sinf(yzAng) * sin(xzAng), cosf(yzAng), 1.0f };
		}

		DirectX::XMVECTOR RandomInUnitSphere()
		{
			float xzAng = DirectX::XM_2PI * rng.Next();
			float yzAng = DirectX::XM_PI * rng.Next();
			float len = rng.Next();
			return { len * sinf(yzAng) * cosf(xzAng), len * sinf(yzAng) * sin(xzAng), len * cosf(yzAng), 1.0f };
		}
	}
}
