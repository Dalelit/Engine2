#pragma once
#include "Texture.h"
#include "AssetStore.h"

namespace Engine2 {

	class TextureLoader
	{
	public:
		static AssetStore<Texture> Textures;

		// Loads a (or returns the cached) texture.
		static std::shared_ptr<Texture> LoadTexture(const std::string& filename);

		static std::string LastResult; // to do: simple first approach to error handling info

	protected:
		static std::shared_ptr<Texture> CreateTexture(const std::string& filename);
	};
}