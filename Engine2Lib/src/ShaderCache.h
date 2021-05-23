#pragma once
#include "AssetStore.h"
#include "Shader.h"

namespace Engine2
{
	class ShaderCache
	{
	public:

		static AssetStore<VertexShader> VertexShaders;
		static AssetStore<PixelShader>  PixelShaders;

		// Retrieve existing, or create and stores, a shader
		static std::shared_ptr<VertexShader> GetVertexShader(const std::string& filename, VertexShaderLayoutDesc& layout);
		static std::shared_ptr<PixelShader>  GetPixelShader(const std::string& filename);

	};
}