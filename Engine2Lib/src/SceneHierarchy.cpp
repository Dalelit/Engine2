#include "pch.h"
#include "Common.h"
#include "SceneHierarchy.h"
#include "submodules/imgui/imgui.h"
#include "VertexLayout.h"
#include "VertexBuffer.h"
#include "MeshRenderer.h"
#include "MaterialLibrary.h"
#include "MeshLoader.h"

using namespace EngineECS;
using namespace DirectX;

namespace Engine2
{
	SceneHierarchy::SceneNode* SceneHierarchy::NewEntity(const std::string& name, SceneNode* parent, SceneNode* insertBefore)
	{
		EntityId_t id = coordinator.CreateEntity();
		if (name.empty()) coordinator.AddComponent<EntityInfo>(id)->tag = std::to_string(id);
		else coordinator.AddComponent<EntityInfo>(id)->tag = name;
		coordinator.AddComponent<Transform>(id);
		coordinator.AddComponent<TransformMatrix>(id);

		if (insertBefore)
		{
			auto& vector = parent ? parent->children : sceneHierarchy;
			size_t index = insertBefore - vector.data();
			auto iter = vector.begin() + index;

			E2_ASSERT(index < vector.size(), "insert position invalid");

			vector.emplace(iter, id);

			selected = &vector[index];
		}
		else
		{
			auto& vector = parent ? parent->children : sceneHierarchy;
			vector.emplace_back(id);
			selected = &vector.back();
		}

		return selected;
	}

	void SceneHierarchy::DestroyEntity(SceneNode* parent, SceneNode* node)
	{
		E2_ASSERT(node, "Null pointer for destroy entity");

		// clean up the children.
		while (node->children.size() > 0)
		{
			DestroyEntity(node, &node->children.back());
		}
		
		coordinator.DestroyEntity(node->id);

		auto& vector = parent ? parent->children : sceneHierarchy;
		auto iter = vector.begin() + (node - vector.data());
		vector.erase(iter);

		selected = parent;
	}

	void SceneHierarchy::UpdateTransformMatrix()
	{
		TransformMatrix matrix(XMMatrixIdentity(), XMMatrixIdentity());

		for (auto& sn : sceneHierarchy)
		{
			UpdateTransformMatrix(sn, matrix);
		}
	}

	void SceneHierarchy::UpdateTransformMatrix(SceneNode& node, TransformMatrix& parentMatrix)
	{
		auto pTransform = coordinator.GetComponent<Transform>(node.id);
		auto pMatrix = coordinator.GetComponent<TransformMatrix>(node.id);

		// update this matrix with it's transform and the parent
		*pMatrix = TransformMatrix(*pTransform) * parentMatrix;

		std::for_each(node.children.begin(), node.children.end(), [&](auto& child) {
			UpdateTransformMatrix(child, *pMatrix);
		});

	}

	void SceneHierarchy::OnImgui()
	{
		onImguiParent = nullptr;
		addEntityParent = nullptr;
		addEntityInsertBefore = nullptr;
		deleteEntity = nullptr;
		deleteEntityParent = nullptr;

		for (auto& sn : sceneHierarchy) SceneNodeOnImGui(sn);

		// deferred create/destroy entity if required
		if (addEntityParent || addEntityInsertBefore) NewEntity(std::string(), addEntityParent, addEntityInsertBefore);

		if (deleteEntity) DestroyEntity(deleteEntityParent, deleteEntity);

		if (ImGui::Button("Add Entity")) NewEntity(std::string());

		if (ImGui::CollapsingHeader("Loader"))
		{
			static char buffer[256] = "Assets\\Models\\Torus.obj";
			ImGui::InputText("filename", buffer, sizeof(buffer));
			if (ImGui::Button("Load")) LoadModel(buffer);
		}

	}

	void SceneHierarchy::SelectedEntityOnImgui()
	{
		if (selected)
		{
			Components::OnImgui(selected->id, coordinator);
		}
	}

	bool SceneHierarchy::LoadModel(const std::string& sourceFilename)
	{
		using Vertex = VertexLayout::PositionNormalColor;
		auto vsLayout = VertexLayout::PositionNormalColor::Layout;

		auto loadedModel = AssetLoaders::ObjLoader::Load(sourceFilename);

		if (!loadedModel) return false;

		auto root = NewEntity(sourceFilename);
		coordinator.GetComponent<EntityInfo>(root->id)->tag = sourceFilename;

		std::map<std::string, std::shared_ptr<MaterialLibrary::PositionNormalColorMaterial>> materials;
		for (auto& [name, data] : loadedModel->materials)
		{
			auto mat = std::make_shared<MaterialLibrary::PositionNormalColorMaterial>();
			auto cb = mat->GetPSCB();
			cb->ambient = data.Ka;
			cb->diffuse = data.Kd;
			cb->emission = data.Ke;
			cb->specular = data.Ks;
			cb->specularExponent = data.Ns;

			materials.insert({ name, mat });
		}

		for (auto& [name, data] : loadedModel->objects)
		{
			auto sn = NewEntity(name, root);
			Entity entity(sn->id, coordinator);

			entity.GetComponent<EntityInfo>()->tag = name;
			auto mr = entity.AddComponent<MeshRenderer>();

			// create the verticies from the loaded model
			std::vector<Vertex> verticies;

			if (data.facesV.size() != data.facesVn.size()) return false;

			verticies.reserve(data.facesV.size()); // get the memory size

			auto pos = data.facesV.data();
			auto nor = data.facesVn.data();
			size_t count = data.facesV.size();

			Vertex v;
			v.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // always white color. shader sets the color
			
			while (count > 0)
			{
				v.position = loadedModel->verticies[*pos];
				v.normal = loadedModel->normals[*nor];
				verticies.push_back(v);
				pos++;
				nor++;
				count--;
			}

			mr->material = materials[data.material];
			mr->mesh = std::make_shared<Mesh>(name);
			auto vb = std::make_shared<VertexBuffer>();
			vb->Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies);
			mr->mesh->SetDrawable(vb);
		}

		return true;
	}

	void SceneHierarchy::Clear()
	{
		while (sceneHierarchy.size() > 0)
			DestroyEntity(nullptr, &sceneHierarchy.back());

		selected = nullptr;
	}

	void SceneHierarchy::SceneNodeOnImGui(SceneNode& node)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (&node == selected) flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)node.id, flags, coordinator.GetComponent<EntityInfo>(node.id)->tag.c_str());

		if (ImGui::IsItemClicked(0))
			selected = &node;

		if (ImGui::BeginPopupContextItem())
		{
			// add entity is deferred until ImGui display is done, otherwise node iterator is invalid

			if (ImGui::MenuItem("Add Child Entity"))
				addEntityParent = &node;
			
			if (ImGui::MenuItem("Insert Entity"))
			{
				addEntityParent = onImguiParent;
				addEntityInsertBefore = &node;
			}

			if (ImGui::MenuItem("Delete Entity"))
			{
				deleteEntityParent = onImguiParent;
				deleteEntity = &node;
			}

			ImGui::EndPopup();
		}
		
		if (open)
		{
			auto parentNode = onImguiParent; // store to revert once finished with children.
			
			onImguiParent = &node;
			for (auto& sn : node.children) SceneNodeOnImGui(sn);
			
			onImguiParent = parentNode; // revert parent

			ImGui::TreePop();
		}
	}
}