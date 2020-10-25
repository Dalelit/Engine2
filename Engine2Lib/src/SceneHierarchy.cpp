#include "pch.h"
#include "Common.h"
#include "SceneHierarchy.h"
#include "Components.h"
#include "submodules/imgui/imgui.h"

using namespace EngineECS;

namespace Engine2
{
	EntityId_t SceneHierarchy::NewEntity(SceneNode* parent, SceneNode* insertBefore)
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

		return id;
	}

	void SceneHierarchy::OnImGui()
	{
		onImguiParent = nullptr;
		addEntityParent = nullptr;
		addEntityInsertBefore = nullptr;

		for (auto& sn : sceneHierarchy) SceneNodeOnImGui(sn);

		// deferred create entity if required
		if (addEntityParent || addEntityInsertBefore) NewEntity(addEntityParent, addEntityInsertBefore);

		if (ImGui::Button("Add Entity")) NewEntity();
	}

	void SceneHierarchy::SelectedEntityOnImGui()
	{
		if (selected)
		{
			Components::OnImgui(selected->id, coordinator);
		}
	}

	void SceneHierarchy::SceneNodeOnImGui(SceneNode& node)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (&node == selected) flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)node.id, flags, coordinator.GetComponent<EntityInfo>(node.id)->tag.c_str());

		if (ImGui::IsItemClicked(0)) selected = &node;

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