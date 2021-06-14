#include "pch.h"
#include "ShaderCache.h"

namespace Engine2
{
	AssetStore<VertexShader> ShaderCache::VertexShaders;
	AssetStore<PixelShader>  ShaderCache::PixelShaders;

	std::shared_ptr<VertexShader> ShaderCache::GetVertexShader(const std::string& filename, VertexShader::VertexShaderLayoutDesc& layout)
	{
		// retrieve it if already loaded
		auto existing = VertexShaders.GetAsset(filename);
		if (existing) return existing;

		// create
		auto ptr = std::make_shared<VertexShader>();
		ptr->CompileFromFile(filename, layout);

		if (!ptr) return nullptr; // failed to load

		// add to the store
		VertexShaders.StoreAsset(filename, ptr);

		return ptr;
	}

	std::shared_ptr<PixelShader> ShaderCache::GetPixelShader(const std::string& filename)
	{
		// retrieve it if already loaded
		auto existing = PixelShaders.GetAsset(filename);
		if (existing) return existing;

		// create
		auto ptr = std::make_shared<PixelShader>();
		ptr->CompileFromFile(filename);

		if (!ptr) return nullptr; // failed to load

		// add to the store
		PixelShaders.StoreAsset(filename, ptr);

		return ptr;
	}
}