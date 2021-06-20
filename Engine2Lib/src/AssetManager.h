#pragma once
#include "pch.h"
#include "AssetStore.h"
#include "Mesh.h"
#include "Materials/StandardMaterial.h"
#include "AssetLoaders/ObjLoader.h"

namespace Engine2
{
	class AssetManager;

	class Asset
	{
	public:

		Asset(const std::string& source) : source(source) {}

		bool LoadModel(const std::string& rootDirectory, const std::string& filename);

		void Clear();

		void OnImgui();

		inline AssetStore<Mesh>& Meshes() { return meshes; }
		inline AssetStore<Materials::StandardMaterial>& Materials() { return materials; }
		inline AssetStore<std::string>& MeshsMaterial() { return meshesMaterial; }

		inline const std::string& GetSource() const { return source; }

	protected:
		friend AssetManager;

		std::string source;
		AssetStore<Mesh> meshes;
		AssetStore<std::string> meshesMaterial;
		AssetStore<Materials::StandardMaterial> materials;

		void CreateMeshAssetPositionNormal(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object);
		void CreateMeshAssetPositionNormalTexture(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object);
		void CreatePositionNormalMaterial(AssetLoaders::ObjLoader& loader);
	};

	using AssetRef = std::reference_wrapper<Asset>;

	class AssetManager
	{
	public:

		using AssetsMap = std::map<std::string, Asset>;

		static inline AssetManager& Manager() { return *instance; }
		static inline std::optional<AssetRef> FindAsset(const std::string& assetName) { return instance->GetAsset(assetName); }

		bool LoadModel(const std::string& rootDirectory, const std::string& filename);

		std::optional<AssetRef> operator[](const std::string& assetName) { return GetAsset(assetName); }

		Asset& CreateAsset(const std::string& assetName);

		void OnImgui();

		void OnImguiSelectMeshPopupOpen();
		std::pair<Asset*, std::shared_ptr<Mesh>> OnImguiSelectMesh();

		void OnImguiSelectMaterialPopupOpen();
		std::pair < Asset*, std::shared_ptr<Materials::StandardMaterial>> OnImguiSelectMaterial();

		std::string OnImguiSelectAssetMenu();

		void Clear();

		AssetsMap& GetMap() { return assets; }

	protected:
		static std::unique_ptr<AssetManager> instance;

		AssetsMap assets;

		std::optional<AssetRef> GetAsset(const std::string& assetName);
	};
}