#include "pch.h"
#include "TextureLoader.h"
#include "SurfaceLoader.h"

namespace Engine2
{
	AssetStore<Texture> TextureLoader::Textures;
	std::string TextureLoader::LastResult;

	std::shared_ptr<Texture> TextureLoader::LoadTexture(const std::string& filename)
	{
		std::shared_ptr<Texture> t = Textures[filename];

		if (t) return t; // already loaded

		t = CreateTexture(filename);

		if (t) Textures.StoreAsset(filename, t);

		return t;
	}

	std::shared_ptr<Texture> TextureLoader::CreateTexture(const std::string& filename)
	{
		auto surface = SurfaceLoader::LoadSurface(filename);

		if (surface)
		{
			auto texture = std::make_shared<Texture>(0, *surface, SurfaceLoader::Format);

			texture->SetName(filename);

			return texture;
		}
		else
		{
			LastResult = SurfaceLoader::LastResult;
			return nullptr;
		}
	}
}
