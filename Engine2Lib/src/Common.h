#pragma once

#include "submodules/imgui/imgui.h"
#include "Config.h"
#include "Util.h"

#define E2_ASSERT(x, msg) assert((x) && msg)
#define E2_ASSERT_HR(hr, msg)   E2_ASSERT(SUCCEEDED(hr), msg);
