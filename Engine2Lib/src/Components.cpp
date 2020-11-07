#include "pch.h"
#include "Components.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"

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

	DirectX::XMMATRIX Transform::Matrix()
	{
		return XMMatrixScalingFromVector(scale) * XMMatrixRotationRollPitchYawFromVector(rotation) * XMMatrixTranslationFromVector(position);
	}

	void Transform::OnImgui()
	{
		static bool lockScale = true;

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			// position
			ImGui::DragFloat3("Position", position.m128_f32, 0.1f);

			// scale
			if (lockScale)
			{
				if (ImGui::DragFloat("Scale", scale.m128_f32, 0.1f))
				{
					scale.m128_f32[1] = scale.m128_f32[0];
					scale.m128_f32[2] = scale.m128_f32[0];
				}
			}
			else
			{
				ImGui::DragFloat3("Scale", scale.m128_f32, 0.1f);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Axis Lock", &lockScale))
			{
				if (lockScale) // changed it to lock the scale
				{
					scale.m128_f32[1] = scale.m128_f32[0];
					scale.m128_f32[2] = scale.m128_f32[0];
				}
			}

			// rotation
			XMVECTOR degs = Math::RadToDeg(rotation);
			if (ImGui::DragFloat3("Rotation", degs.m128_f32, 0.5f)) rotation = Math::DegToRad(degs);
			
			if (ImGui::Button("Reset"))
			{
				position = { 0.0f, 0.0f, 0.0f, 1.0f };
				scale = { 1.0f, 1.0f, 1.0f, 0.0f };
				rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
			}

			ImGui::TreePop();
		}
	}

	void TransformMatrix::OnImgui()
	{
		if (ImGui::TreeNodeEx("TransformMatrix"))
		{
			XMVECTOR vScale, vRotQ, vTrans, vRotEulerDeg, vRotEulerDegNew;
			XMMatrixDecompose(&vScale, &vRotQ, &vTrans, XMMatrixTranspose(matrix));
			vRotEulerDegNew = vRotEulerDeg = Math::RadToDeg(Math::QuaternionToEuler(vRotQ));

			ImGui::DragFloat3("Position", vTrans.m128_f32, 0.1f);
			ImGui::DragFloat3("Scale", vScale.m128_f32, 0.1f);
			ImGui::DragFloat3("Roll/Pitch/Yaw", vRotEulerDegNew.m128_f32, 0.5f);

			ImGui::TreePop();
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
	inline void ComponentOnImgui(const std::string& displayName, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (coord.HasComponent<T>(id))
		{
			bool open = ImGui::TreeNodeEx(displayName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen);
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
	inline void AddComponentOnImgui(const std::string& displayName, EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (!coord.HasComponent<T>(id) && ImGui::Selectable(displayName.c_str())) coord.AddComponent<T>(id);
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

		if (ImGui::BeginCombo("Add Component", ""))
		{
			AddComponentOnImgui<RigidBody>("RigidBody", id, coord);
			AddComponentOnImgui<MeshRenderer>("MeshRenderer", id, coord);
			AddComponentOnImgui<PointLight>("PointLight", id, coord);
			AddComponentOnImgui<ParticleEmitter>("ParticleEmitter", id, coord);
			AddComponentOnImgui<Gizmo>("Gizmo", id, coord);
			AddComponentOnImgui<OffscreenOutliner>("Outliner", id, coord);

			ImGui::EndCombo();
		}
	}
}