#pragma once
#include "AssetLoaders/ObjLoader.h"

namespace Engine2
{
	class MeshAssetLoader
	{
	public:
		static std::vector<std::string> CreateMeshAssetPositionNormalColor(AssetLoaders::ObjLoader& loader);
		static std::vector<std::string> CreateMeshAssetPositionNormalTexture(AssetLoaders::ObjLoader& loader);
	};
}
