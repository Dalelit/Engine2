#include "pch.h"
#include "MeshLoader.h"
#include "VertexLayout.h"
#include "Common.h"
#include "Mesh.h"
#include "VertexBuffer.h"

namespace Engine2
{
	std::vector<std::string> MeshAssetLoader::CreateMeshAssetPositionNormalColor(AssetLoaders::ObjLoader& loader)
	{
		using Vertex = VertexLayout::PositionNormalColor;

		std::vector<std::string> names;

		for (auto& [name, data] : loader.objects)
		{
			// create the verticies from the loaded model
			std::vector<Vertex> verticies;

			E2_ASSERT(data.facesV.size() == data.facesVn.size(), "Loaded model position and normal counts do not align");

			verticies.reserve(data.facesV.size()); // get the memory size

			auto pos = data.facesV.data();
			auto nor = data.facesVn.data();
			size_t count = data.facesV.size();

			Vertex v;
			v.color = { 1.0f, 1.0f, 1.0f, 1.0f };

			while (count > 0)
			{
				v.position = loader.verticies[*pos];
				v.normal = loader.normals[*nor];
				verticies.push_back(v);
				pos++;
				nor++;
				count--;
			}

			// create the asset
			auto m = Mesh::Assets.CreateAsset(name);
			m->SetName(name);
			names.push_back(name);
			m->SetDrawable<MeshTriangleList<Vertex>>(verticies);
		}

		return names;
	}

	std::vector<std::string> MeshAssetLoader::CreateMeshAssetPositionNormalTexture(AssetLoaders::ObjLoader& loader)
	{
		using Vertex = VertexLayout::PositionNormalTexture;

		std::vector<std::string> names;

		for (auto& [name, data] : loader.objects)
		{
			// create the verticies from the loaded model
			std::vector<Vertex> verticies;

			E2_ASSERT(data.facesV.size() == data.facesVn.size(), "Loaded model position and normal counts do not align");
			E2_ASSERT(data.facesV.size() == data.facesVt.size(), "Loaded model position and textcoord counts do not align");

			verticies.reserve(data.facesV.size()); // get the memory size

			auto pos = data.facesV.data();
			auto nor = data.facesVn.data();
			auto tex = data.facesVt.data();
			size_t count = data.facesV.size();

			Vertex v;

			while (count > 0)
			{
				v.position = loader.verticies[*pos];
				v.normal = loader.normals[*nor];
				v.texcoord = loader.textureCoords[*tex];
				verticies.push_back(v);
				pos++;
				nor++;
				tex++;
				count--;
			}

			// create the asset
			auto m = Mesh::Assets.CreateAsset(name + "_t");
			m->SetName(name);
			names.push_back(name);
			m->SetDrawable<MeshTriangleList<Vertex>>(verticies);
		}

		return names;
	}
}
