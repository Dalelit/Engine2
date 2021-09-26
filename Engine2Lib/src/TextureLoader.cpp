#include "pch.h"
#include "TextureLoader.h"
#include "SurfaceLoader.h"

namespace Engine2
{
	AssetStore<Texture> TextureLoader::Textures;
	std::string TextureLoader::LastResult;

	std::shared_ptr<Texture> TextureLoader::LoadTexture(const std::string& directory, const std::string& filename)
	{
		std::shared_ptr<Texture> t = Textures[filename];

		if (t) return t; // already loaded

		t = CreateTexture(directory, filename);

		if (t) Textures.StoreAsset(filename, t);

		return t;
	}

	std::shared_ptr<Texture> TextureLoader::CreateTexture(const std::string& directory, const std::string& filename)
	{
		auto surface = SurfaceLoader::LoadSurface(directory + "\\" + filename);

		if (surface)
		{
			auto texture = std::make_shared<Texture>(0, *surface, SurfaceLoader::Format);

			texture->SetName(filename);
			texture->SetSampler(TextureSampler::CreateDefault());

			return texture;
		}
		else
		{
			LastResult = SurfaceLoader::LastResult;
			return nullptr;
		}
	}
}
