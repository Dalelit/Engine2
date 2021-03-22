#include "pch.h"
#include "Components.h"
#include "EntityInfo.h"
#include "Transform.h"
#include "Gizmo.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"
#include "MeshRenderer.h"
#include "MeshComponent.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"
#include "Camera.h"
#include "ScriptComponent.h"

using namespace DirectX;

namespace Engine2
{
	//
	// On component add. Called after component is created.
	// Override for specific scenarios.
	//
	template <typename T>
	void OnComponentAdded(T* componentPtr, EngineECS::EntityId_t id, EngineECS::Coordinator& coord) { }

	template <>
	void OnComponentAdded(MeshComponent* componentPtr, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (coord.HasComponent<MeshRenderer>(id)) coord.GetComponent<MeshRenderer>(id)->SetMesh(componentPtr->GetMesh());
	}

	//
	// On component about to be removed. Called just before component is removed and destroyed.
	// Override for specific scenarios.
	//
	template <typename T>
	void OnComponentToBeRemoved(T* componentPtr, EngineECS::EntityId_t id, EngineECS::Coordinator& coord) { }

	template <>
	void OnComponentToBeRemoved(MeshComponent* componentPtr, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (coord.HasComponent<MeshRenderer>(id)) coord.GetComponent<MeshRenderer>(id)->ClearMesh();
	}

	//
	// on component imgui
	//
	template <typename T>
	void ComponentOnImgui(const char* displayName, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (coord.HasComponent<T>(id))
		{
			bool open = ImGui::TreeNodeEx(displayName, ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen);
			bool destroy = false;

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Destroy component")) destroy = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				coord.GetComponent<T>(id)->OnImgui();
				ImGui::TreePop();
			}

			if (destroy)
			{
				OnComponentToBeRemoved<T>(coord.GetComponent<T>(id), id, coord);
				coord.DestroyComponent<T>(id);
			}
		}
	}

	template <typename T>
	inline T* AddComponentOnImgui(const char* displayName, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (!coord.HasComponent<T>(id) && ImGui::Selectable(displayName))
		{
			auto comp = coord.AddComponent<T>(id);
			OnComponentAdded<T>(comp, id, coord);
			return comp;
		}
		else
		{
			return nullptr;
		}
	}

	void Components::OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		coord.GetComponent<EntityInfo>(id)->OnImgui();
		coord.GetComponent<Transform>(id)->OnImgui();
		coord.GetComponent<TransformMatrix>(id)->OnImgui();
		ComponentOnImgui<MeshRenderer>("MeshRenderer", id, coord);
		ComponentOnImgui<MeshComponent>("MeshComponent", id, coord);
		ComponentOnImgui<RigidBody>("RigidBody", id, coord);
		ComponentOnImgui<Collider>("Collider", id, coord);
		ComponentOnImgui<PointLight>("PointLight", id, coord);
		ComponentOnImgui<ParticleEmitter>("ParticleEmitter", id, coord);
		ComponentOnImgui<Gizmo>("Gizmo", id, coord);
		ComponentOnImgui<OffscreenOutliner>("Outliner", id, coord);
		ComponentOnImgui<Camera>("Camera", id, coord);
		ComponentOnImgui<ScriptComponent>("Scripts", id, coord);
		ComponentOnImgui<Offscreen>("Offscreen", id, coord);

		if (ImGui::BeginCombo("Add Component", ""))
		{
			AddComponentOnImgui<MeshRenderer>("MeshRenderer", id, coord);
			AddComponentOnImgui<MeshComponent>("MeshComponent", id, coord);
			AddComponentOnImgui<RigidBody>("RigidBody", id, coord);
			AddComponentOnImgui<Collider>("Collider", id, coord);
			AddComponentOnImgui<PointLight>("PointLight", id, coord);
			AddComponentOnImgui<ParticleEmitter>("ParticleEmitter", id, coord);
			AddComponentOnImgui<Gizmo>("Gizmo", id, coord);
			AddComponentOnImgui<OffscreenOutliner>("Outliner", id, coord);
			AddComponentOnImgui<Camera>("Camera", id, coord);
			AddComponentOnImgui<Offscreen>("Offscreen", id, coord);

			// Script Components need and extra step of setting the entity
			//AddComponentOnImgui<ScriptComponent>("Scripts", id, coord);
			if (!coord.HasComponent<ScriptComponent>(id) && ImGui::Selectable("Scripts"))
			{
				auto* sc = coord.AddComponent<ScriptComponent>(id);
				sc->SetEntity(Entity(id, coord));
			}

			ImGui::EndCombo();
		}
	}
}