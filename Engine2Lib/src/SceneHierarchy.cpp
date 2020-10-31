#include "pch.h"
#include "Common.h"
#include "SceneHierarchy.h"
#include "Components.h"
#include "submodules/imgui/imgui.h"
#include "VertexLayout.h"
#include "VertexBuffer.h"
#include "MeshRenderer.h"

using namespace EngineECS;

namespace Engine2
{
	SceneHierarchy::SceneNode* SceneHierarchy::NewEntity(SceneNode* parent, SceneNode* insertBefore)
	{
		EntityId_t id = coordinator.CreateEntity();
		coordinator.AddComponent<EntityInfo>(id)->tag = std::to_string(id);
		coordinator.AddComponent<Transform>(id);

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

	void SceneHierarchy::OnImGui()
	{
		onImguiParent = nullptr;
		addEntityParent = nullptr;
		addEntityInsertBefore = nullptr;
		deleteEntity = nullptr;
		deleteEntityParent = nullptr;

		for (auto& sn : sceneHierarchy) SceneNodeOnImGui(sn);

		// deferred create/destroy entity if required
		if (addEntityParent || addEntityInsertBefore) NewEntity(addEntityParent, addEntityInsertBefore);

		if (deleteEntity) DestroyEntity(deleteEntityParent, deleteEntity);

		if (ImGui::Button("Add Entity")) NewEntity();

		if (ImGui::CollapsingHeader("Loader"))
		{
			static char buffer[256] = "Assets\\Models\\Torus.obj";
			ImGui::InputText("filename", buffer, sizeof(buffer));
			if (ImGui::Button("Load")) LoadModel(buffer);
		}

	}

	void SceneHierarchy::SelectedEntityOnImGui()
	{
		if (selected)
		{
			Components::OnImgui(selected->id, coordinator);
		}
	}

	bool SceneHierarchy::LoadModel(const std::string& sourceFilename)
	{
		using Vertex = VertexLayout::PositionNormalColor::Vertex;
		auto vsLayout = VertexLayout::PositionNormalColor::GetLayout();

		auto loadedModel = AssetLoaders::ObjLoader::Load(sourceFilename);

		if (!loadedModel) return false;

		// To Do: will sort this out later.
		auto material = Material::Assets["Default"];

		auto root = NewEntity();
		coordinator.GetComponent<EntityInfo>(root->id)->tag = sourceFilename;

		for (auto& [name, data] : loadedModel->objects)
		{
			auto sn = NewEntity(root);
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
			if (loadedModel->materials.find(data.material) == loadedModel->materials.end())
			{
				v.color = { 1.0f, 1.0f, 1.0f, 1.0f };
			}
			else
			{
				v.color.x = loadedModel->materials[data.material].Kd.x;
				v.color.y = loadedModel->materials[data.material].Kd.y;
				v.color.z = loadedModel->materials[data.material].Kd.z;
				v.color.w = 1.0f;
			}

			while (count > 0)
			{
				v.position = loadedModel->verticies[*pos];
				v.normal = loadedModel->normals[*nor];
				verticies.push_back(v);
				pos++;
				nor++;
				count--;
			}

			mr->material = material;
			mr->mesh = std::make_shared<Mesh>(name);
			mr->mesh->SetDrawable<MeshTriangleList<Vertex>>(verticies);
		}

		return true;
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