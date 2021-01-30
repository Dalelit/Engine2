#pragma once

#include "submodules/imgui/imgui.h"
#include "Config.h"
#include "Util.h"
#include "Logging.h"

#define E2_ASSERT(x, msg) assert((x) && msg)
#define E2_ASSERT_HR(hr, msg)   E2_ASSERT(SUCCEEDED(hr), msg);

namespace Engine2
{
	inline std::ostream& operator<<(std::ostream& strm, DirectX::XMVECTOR& v) { strm << v.m128_f32[0] << ", " << v.m128_f32[1] << ", " << v.m128_f32[2] << ", " << v.m128_f32[3]; return strm; }
}