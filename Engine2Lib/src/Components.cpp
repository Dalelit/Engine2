#include "pch.h"
#include "Components.h"
#include "Transform.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"
#include "Camera.h"
#include "ScriptComponent.h"


using namespace DirectX;

namespace Engine2
{
	void EntityInfo::OnImgui()
	{
		char buffer[256] = {};
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tag = buffer;
		}
	}

	void Gizmo::OnImgui()
	{
		const char* selected = "";

		switch (type)
		{
			case Types::Axis: selected = "Axis"; break;
			case Types::Cube: selected = "Cube"; break;
			case Types::Sphere: selected = "Sphere"; break;
			case Types::Camera: selected = "Camera"; break;
		}

		if (ImGui::BeginCombo("Type", selected))
		{
			if (ImGui::Selectable("Axis",   type == Types::Axis)) type = Types::Axis;
			if (ImGui::Selectable("Cube",   type == Types::Cube)) type = Types::Cube;
			if (ImGui::Selectable("Sphere", type == Types::Sphere)) type = Types::Sphere;
			if (ImGui::Selectable("Camera", type == Types::Camera)) type = Types::Camera;
			ImGui::EndCombo();
		}
	}

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
				coord.DestroyComponent<T>(id);
			}
		}
	}

	template <typename T>
	inline void AddComponentOnImgui(const char* displayName, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (!coord.HasComponent<T>(id) && ImGui::Selectable(displayName)) coord.AddComponent<T>(id);
	}

	void Components::OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		coord.GetComponent<EntityInfo>(id)->OnImgui();
		coord.GetComponent<Transform>(id)->OnImgui();
		coord.GetComponent<TransformMatrix>(id)->OnImgui();
		ComponentOnImgui<RigidBody>("RigidBody", id, coord);
		ComponentOnImgui<MeshRenderer>("MeshRenderer", id, coord);
		ComponentOnImgui<PointLight>("PointLight", id, coord);
		ComponentOnImgui<ParticleEmitter>("ParticleEmitter", id, coord);
		ComponentOnImgui<Gizmo>("Gizmo", id, coord);
		ComponentOnImgui<OffscreenOutliner>("Outliner", id, coord);
		ComponentOnImgui<Camera>("Camera", id, coord);
		ComponentOnImgui<ScriptComponent>("Scripts", id, coord);

		if (ImGui::BeginCombo("Add Component", ""))
		{
			AddComponentOnImgui<RigidBody>("RigidBody", id, coord);
			AddComponentOnImgui<MeshRenderer>("MeshRenderer", id, coord);
			AddComponentOnImgui<PointLight>("PointLight", id, coord);
			AddComponentOnImgui<ParticleEmitter>("ParticleEmitter", id, coord);
			AddComponentOnImgui<Gizmo>("Gizmo", id, coord);
			AddComponentOnImgui<OffscreenOutliner>("Outliner", id, coord);
			AddComponentOnImgui<Camera>("Camera", id, coord);
			
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