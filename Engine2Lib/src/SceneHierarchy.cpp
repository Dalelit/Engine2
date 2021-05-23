#include "pch.h"
#include "Common.h"
#include "SceneHierarchy.h"
#include "submodules/imgui/imgui.h"
#include "EntityInfo.h"
#include "VertexLayout.h"
#include "VertexBuffer.h"
#include "MeshRenderer.h"

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

	void SceneHierarchy::CloneEntity(SceneNode* parent, SceneNode* node)
	{
		EntityId_t id = coordinator.CloneEntity(node->id);

		auto& vector = parent ? parent->children : sceneHierarchy;
		vector.emplace_back(id);
		selected = &vector.back();

		for (auto& child : node->children)
		{
			CloneEntity(node, &child);
		}
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
		cloneEntity = nullptr;
		cloneEntityParent = nullptr;

		for (auto& sn : sceneHierarchy) SceneNodeOnImGui(sn);

		// deferred create/destroy entity if required
		if (addEntityParent || addEntityInsertBefore) NewEntity(std::string(), addEntityParent, addEntityInsertBefore);

		if (deleteEntity) DestroyEntity(deleteEntityParent, deleteEntity);

		if (cloneEntity) CloneEntity(cloneEntityParent, cloneEntity);

		if (ImGui::Button("Add Entity")) NewEntity(std::string());
	}

	void SceneHierarchy::SelectedEntityOnImgui()
	{
		if (selected)
		{
			Components::OnImgui(selected->id, coordinator);
		}
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

			if (ImGui::MenuItem("Clone Entity"))
			{
				cloneEntityParent = onImguiParent;
				cloneEntity = &node;
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