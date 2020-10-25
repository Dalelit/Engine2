#include "pch.h"
#include "Mesh.h"
#include "VertexLayout.h"
#include "VertexBuffer.h"

namespace Engine2
{
	AssetStore<Mesh> Mesh::Assets;

	void Mesh::OnImgui(bool assetInfo)
	{
		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen, "Mesh %s", name.c_str()))
		{
			if (assetInfo) ImGui::Text("Drawable references %i", drawable.use_count());

			if (drawable) drawable->OnImgui();
			else ImGui::Text("Drawable null");

			ImGui::TreePop();
		}
	}

	std::vector<std::string> MeshAssetLoader::CreateMeshAsset(AssetLoaders::ObjLoader& loader)
	{
		using Vertex = VertexLayout::PositionNormalColor::Vertex;
		auto vsLayout = VertexLayout::PositionNormalColor::GetLayout();

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
			names.push_back(name);
			m->SetDrawable<MeshTriangleList<Vertex>>(verticies);
		}

		return names;
	}
}