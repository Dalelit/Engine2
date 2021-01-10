#include "pch.h"
#include "AssetManager.h"
#include "UtilFileDialog.h"
#include "AssetLoaders/ObjLoader.h"
#include "VertexLayout.h"
#include "VertexBuffer.h"
#include "MaterialLibrary.h"

namespace Engine2
{
	std::unique_ptr<AssetManager> AssetManager::instance = std::make_unique<AssetManager>();

	bool AssetManager::LoadModel(const std::string& filename)
	{
		lastActionResult.clear();

		auto iter = assets.find(filename);

		if (iter == assets.end())
		{
			lastActionResult = "New asset";
			iter = assets.emplace(filename, filename).first;
		}
		else
		{
			lastActionResult = "Overridden asset";
			iter->second.Clear();
		}

		if (iter->second.LoadModel(filename))
		{
			lastActionResult += " success";
			return true;
		}
		else
		{
			lastActionResult += " failed";
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

	bool Asset::LoadModel(const std::string& filename)
	{
		auto loadedModel = AssetLoaders::ObjLoader::Load(filename);

		if (loadedModel->IsValid())
		{
			CreatePositionNormalColorMaterial(*loadedModel);

			for (auto& [name, object] : loadedModel->objects)
			{
				if (object.HasPositionNormalTexture())
				{
					CreateMeshAssetPositionNormalTexture(*loadedModel, object);
				}
				else if (object.HasPositionNormal())
				{
					CreateMeshAssetPositionNormalColor(*loadedModel, object);
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
	}

	void Asset::CreateMeshAssetPositionNormalColor(AssetLoaders::ObjLoader& loader, AssetLoaders::Object& object)
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
		auto vb = std::make_shared<VertexBuffer>();
		vb->Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies);
		m->SetDrawable(vb);
	}

	void Asset::CreatePositionNormalColorMaterial(AssetLoaders::ObjLoader& loader)
	{
		for (auto& [name, data] : loader.materials)
		{
			auto mat = std::make_shared<MaterialLibrary::PositionNormalColorMaterial>();
			auto cb = mat->GetPSCB();
			cb->ambient = data.Ka;
			cb->diffuse = data.Kd;
			cb->emission = data.Ke;
			cb->specular = data.Ks;
			cb->specularExponent = data.Ns;

			materials.StoreAsset(name, mat);
		}
	}

	void AssetManager::OnImgui()
	{
		static std::string selectedFile = "Assets//Models//Primatives.obj";
		ImGui::Text("File: %s", selectedFile.c_str());
		if (ImGui::Button("Load file..."))
		{
			if (Util::FileSelectionDialogue::LoadDialogue(selectedFile))
				LoadModel(selectedFile);
		}
		if (ImGui::Button("Reload"))
		{
			LoadModel(selectedFile);
		}

		if (!lastActionResult.empty()) ImGui::Text("%s", lastActionResult.c_str());

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

	std::shared_ptr<Mesh> AssetManager::OnImguiSelectMesh()
	{
		if (ImGui::BeginPopup("Meshes::AssetManager"))
		{
			std::shared_ptr<Mesh> result;

			for (auto& [ak, av] : assets)
			{
				if (ImGui::BeginMenu(ak.c_str()))
				{
					for (auto& [mk, mv] : av.meshes.Map())
					{
						if (ImGui::MenuItem(mk.c_str()))
						{
							result = mv;
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndPopup();

			return result;
		}
		else
		{
			return nullptr;
		}
	}

	void AssetManager::OnImguiSelectMaterialPopupOpen()
	{
		ImGui::OpenPopup("Material::AssetManager");
	}

	std::shared_ptr<Material> AssetManager::OnImguiSelectMaterial()
	{
		if (ImGui::BeginPopup("Material::AssetManager"))
		{
			std::shared_ptr<Material> result;

			for (auto& [ak, av] : assets)
			{
				if (ImGui::BeginMenu(ak.c_str()))
				{
					for (auto& [mk, mv] : av.materials.Map())
					{
						if (ImGui::MenuItem(mk.c_str()))
						{
							result = mv;
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndPopup();

			return result;
		}
		else
		{
			return nullptr;
		}
	}

	void AssetManager::Clear()
	{
		assets.clear();
	}

}