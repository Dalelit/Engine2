#include "pch.h"
#include "Config.h"

namespace Engine2
{
	std::map<std::string, std::string> Config::directories = {
		{ "ShaderCompiledDir", "..\\x64\\Shaders\\" },
		{ "ShaderSourceDir", "Assets\\Materials\\" },
		{ "ModelsDir", "Assets\\Models\\" },
		{ "EngineShaderSourceDir", "..\\Engine2Lib\\Assets\\Shaders\\" },
	};
}
