#include "pch.h"
#include "AssetManager.h"
#include "UtilFileDialog.h"
#include "AssetLoaders/ObjLoader.h"
#include "VertexLayout.h"
#include "VertexBuffer.h"
#include "Materials/StandardMaterial.h"
#include "Logging.h"

namespace Engine2
{
	std::unique_ptr<AssetManager> AssetManager::instance = std::make_unique<AssetManager>();

	bool AssetManager::LoadModel(const std::string& rootDirectory, const std::string& filename)
	{
		E2_LOG_INFO("Loading model " + filename);

		std::string result;

		auto iter = assets.find(filename);

		if (iter == assets.end())
		{
			result = "New asset";
			iter = assets.emplace(filename, filename).first;
		}
		else
		{
			result = "Overridden asset";
			iter->second.Clear();
		}

		if (iter->second.LoadModel(rootDirectory, filename))
		{
			result += " success";
			E2_LOG_INFO(result);
			return true;
		}
		else
		{
			result += " failed";
			E2_LOG_WARNING(result);
			return false;
		}
	}

	std::optional<AssetRef> AssetManager::GetAsset(const std::string& assetName)
	{
		auto iter = assets.find(assetName);

		return iter == assets.end() ? std::nullopt
									: std::optional<AssetRef>{ iter->second };
	}

	Asset& AssetManager::CreateAsset(const std::string& assetName)
	{
		auto result = assets.emplace(assetName, assetName);

		E2_ASSERT(result.second, "Attempted to create an asset that already exists");

		return result.first->second;
	}

	bool Asset::LoadModel(const std::string& rootDirectory, const std::string& filename)
	{
		auto loadedModel = AssetLoaders::ObjLoader::Load(rootDirectory, filename);

		if (loadedModel->IsValid())
		{
			CreatePositionNormalMaterial(*loadedModel);

			for (auto& model : loadedModel->models)
			{
				for (auto& [name, object] : model.objects)
				{
					if (object.HasPositionNormalTexture())
					{
						CreateMeshAssetPositionNormalTexture(*loadedModel, object);
					}
					else if (object.HasPositionNormal())
					{
						CreateMeshAssetPositionNormal(*loadedModel, object);
					}
				}
			}
		}

		return meshes.Size() > 0;
	}

	void Asset::Clear()
	{
		source.clear();
		meshes.Clear();
		materials.Clear();
	}

	void Asset::OnImgui()
	{
		ImGui::Text("Source: %s", source.c_str());
		if (ImGui::TreeNode("Meshes"))
		{
			meshes.OnImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Materials"))
		{
			materials.OnImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Mesh to Material"))
		{
			for (auto& [k,v] : meshesMaterial.Map()) ImGui::Text("%s   %s", k.c_str(), v->c_str());

			ImGui::TreePop();
		}
	}

	void Asset::CreateMeshAssetPositionNormal(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object)
	{
		using Vertex = VertexLayout::PositionNormalColor;

		// create the verticies from the loaded model
		std::vector<Vertex> verticies;

		E2_ASSERT(object.facesV.size() == object.facesVn.size(), "Loaded model position and normal counts do not align");

		verticies.reserve(object.facesV.size()); // get the memory size

		auto pos = object.facesV.data();
		auto nor = object.facesVn.data();
		size_t count = object.facesV.size();

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
		auto m = meshes.CreateAsset(object.name);
		m->SetName(object.name); // to do: shouldn't need to do this!
		meshesMaterial.CreateAsset(object.name, object.material); // track the material for the mesh from the source
		auto vb = std::make_shared<VertexBuffer>();
		vb->Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies);
		m->SetDrawable(vb);
	}

	void Asset::CreateMeshAssetPositionNormalTexture(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object)
	{
		using Vertex = VertexLayout::PositionNormalTexture;

		// create the verticies from the loaded model
		std::vector<Vertex> verticies;

		E2_ASSERT(object.facesV.size() == object.facesVn.size(), "Loaded model position and normal counts do not align");
		E2_ASSERT(object.facesV.size() == object.facesVt.size(), "Loaded model position and textcoord counts do not align");

		verticies.reserve(object.facesV.size()); // get the memory size

		auto pos = object.facesV.data();
		auto nor = object.facesVn.data();
		auto tex = object.facesVt.data();
		size_t count = object.facesV.size();

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
		auto m = meshes.CreateAsset(object.name);
		m->SetName(object.name); // to do: shouldn't need to do this!
		meshesMaterial.CreateAsset(object.name, object.material); // track the material for the mesh from the source
		auto vb = std::make_shared<VertexBuffer>();
		vb->Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies);
		m->SetDrawable(vb);
	}

	void Asset::CreatePositionNormalMaterial(AssetLoaders::ObjLoader& loader)
	{
		for (auto& [name, data] : loader.materials)
		{
			auto mat = std::make_shared<Materials::StandardMaterial>(name);
			//auto& cb = mat->GetMaterialData();
			Materials::StandardPSData psdata;
			psdata.ambient = data.Ka;
			psdata.diffuse = data.Kd;
			psdata.emission = data.Ke;
			psdata.specular = data.Ks;
			psdata.specularExponent = data.Ns;
			mat->SetMaterialData(psdata);

			materials.StoreAsset(name, mat);
		}
	}

	void AssetManager::OnImgui()
	{
		if (ImGui::TreeNode("Assets"))
		{
			for (auto& [name, asset] : assets)
			{
				if (ImGui::TreeNode(name.c_str()))
				{
					asset.OnImgui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}

	void AssetManager::OnImguiSelectMeshPopupOpen()
	{
		ImGui::OpenPopup("Meshes::AssetManager");
	}

	std::pair<Asset*, std::shared_ptr<Mesh>> AssetManager::OnImguiSelectMesh()
	{
		std::pair<Asset*, std::shared_ptr<Mesh>> result(nullptr, nullptr);

		if (ImGui::BeginPopup("Meshes::AssetManager"))
		{
			for (auto& [ak, av] : assets)
			{
				if (ImGui::BeginMenu(ak.c_str()))
				{
					for (auto& [mk, mv] : av.meshes.Map())
					{
						if (ImGui::MenuItem(mk.c_str()))
						{
							result.first = &av;
							result.second = mv;
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndPopup();
		}

		return result;
	}

	void AssetManager::OnImguiSelectMaterialPopupOpen()
	{
		ImGui::OpenPopup("Material::AssetManager");
	}

	std::pair <Asset*, std::shared_ptr<Materials::StandardMaterial>> AssetManager::OnImguiSelectMaterial()
	{
		std::pair < Asset*, std::shared_ptr<Materials::StandardMaterial>> result(nullptr, nullptr);

		if (ImGui::BeginPopup("Material::AssetManager"))
		{
			for (auto& [ak, av] : assets)
			{
				if (ImGui::BeginMenu(ak.c_str()))
				{
					for (auto& [mk, mv] : av.materials.Map())
					{
						if (ImGui::MenuItem(mk.c_str()))
						{
							result.first = &av;
							result.second = mv;
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndPopup();
		}

		return result;
	}

	std::string AssetManager::OnImguiSelectAssetMenu()
	{
		std::string result;

		for (auto& [ak, av] : assets)
		{
			if (ImGui::MenuItem(ak.c_str()))
			{
				result = ak;
			}
		}

		return result;
	}

	void AssetManager::Clear()
	{
		assets.clear();
	}

}