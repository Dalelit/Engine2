#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetLoaders/ObjLoader.h"

namespace Engine2
{
	class AssetManager;

	class Asset
	{
	public:

		//Asset() = default;
		Asset(const std::string& source) : source(source) {}

		bool LoadModel(const std::string& filename);

		void Clear();

		void OnImgui();

		inline AssetStore<Mesh>& Meshes() { return meshes; }
		inline AssetStore<Material>& Materials() { return materials; }

	protected:
		friend AssetManager;

		std::string source;
		AssetStore<Mesh> meshes;
		AssetStore<std::string> meshesMaterial;
		AssetStore<Material> materials;

		void CreateMeshAssetPositionNormalColor(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object);
		void CreateMeshAssetPositionNormalTexture(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object);
		void CreatePositionNormalColorMaterial(AssetLoaders::ObjLoader& loader);
	};

	using AssetRef = std::reference_wrapper<Asset>;

	class AssetManager
	{
	public:

		static inline AssetManager& Manager() { return *instance; }
		static inline std::optional<AssetRef> FindAsset(const std::string& assetName) { return instance->GetAsset(assetName); }

		bool LoadModel(const std::string& filename);

		std::optional<AssetRef> operator[](const std::string& assetName) { return GetAsset(assetName); }

		Asset& CreateAsset(const std::string& assetName);

		void OnImgui();

		void OnImguiSelectMeshPopupOpen();
		std::shared_ptr<Mesh> OnImguiSelectMesh();

		void OnImguiSelectMaterialPopupOpen();
		std::shared_ptr<Material> OnImguiSelectMaterial();

		void Clear();

	protected:
		static std::unique_ptr<AssetManager> instance;

		using AssetsMap = std::map<std::string, Asset>;
		AssetsMap assets;

		std::string lastActionResult;

		std::optional<AssetRef> GetAsset(const std::string& assetName);
	};
}