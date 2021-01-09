#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"

namespace Engine2
{
	class Asset
	{
	public:

		void OnImgui();

	protected:
		std::string source;
		AssetStore<Mesh> meshes;
		AssetStore<Material> materials;
	};

	using AssetRef = std::reference_wrapper<Asset>;

	class AssetManager
	{
	public:

		bool LoadModel(const std::string& filename);

		std::optional<AssetRef> GetAsset(const std::string& assetName);
		std::optional<AssetRef> operator[](const std::string& assetName) { return GetAsset(assetName); }

		void OnImgui();

	protected:

		using AssetsMap = std::map<std::string, Asset>;
		AssetsMap assets;
	};
}