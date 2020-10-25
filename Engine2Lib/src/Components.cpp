#include "pch.h"
#include "Components.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"

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

	void Transform::Set(float positionX, float positionY, float positionZ, float scaleX, float scaleY, float scaleZ, float rollDeg, float pitchDeg, float yawDeg)
	{
		transform = XMMatrixScaling(scaleX, scaleY, scaleZ);
		transform *= XMMatrixRotationRollPitchYaw(Math::DegToRad(pitchDeg), Math::DegToRad(yawDeg), Math::DegToRad(rollDeg));
		transform *= XMMatrixTranslation(positionX, positionY, positionZ);
		transform = XMMatrixTranspose(transform);
	}

	void Transform::OnImgui()
	{
		// to do:
		// - scale going negative gets messed up... assuming it's to do with the decompose?
		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			XMVECTOR vScale, vRotQ, vTrans, vRotEulerDeg, vRotEulerDegNew;
			XMMatrixDecompose(&vScale, &vRotQ, &vTrans, XMMatrixTranspose(transform));
			vRotEulerDegNew = vRotEulerDeg = Math::RadToDeg(Math::QuaternionToEuler(vRotQ));

			auto MatrixCompose = [&]() {
				transform = XMMatrixScalingFromVector(vScale);
				transform *= XMMatrixRotationQuaternion(vRotQ);
				transform *= XMMatrixTranslationFromVector(vTrans);
				transform = XMMatrixTranspose(transform);
			};

			if (ImGui::DragFloat3("Position", vTrans.m128_f32, 0.1f)) MatrixCompose();
			if (ImGui::DragFloat3("Scale", vScale.m128_f32, 0.1f))    MatrixCompose();
			if (ImGui::DragFloat3("Roll/Pitch/Yaw", vRotEulerDegNew.m128_f32, 0.5f))
			{
				XMVECTOR qChange = XMQuaternionRotationRollPitchYawFromVector(Math::DegToRad(vRotEulerDegNew - vRotEulerDeg));
				vRotQ = XMQuaternionMultiply(vRotQ, qChange);
				MatrixCompose();
			}
			
			if (ImGui::Button("Reset")) { transform = XMMatrixIdentity(); }

			ImGui::TreePop();
		}
	}

	void Gizmo::OnImgui()
	{
		if (ImGui::TreeNodeEx("Gizmo", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
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
			ImGui::TreePop();
		}
	}

	void Components::OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
	{
		if (coord.HasComponent<EntityInfo>(id)) coord.GetComponent<EntityInfo>(id)->OnImgui();
		if (coord.HasComponent<Transform>(id)) coord.GetComponent<Transform>(id)->OnImgui();
		if (coord.HasComponent<RigidBody>(id)) coord.GetComponent<RigidBody>(id)->OnImgui();
		if (coord.HasComponent<MeshRenderer>(id)) coord.GetComponent<MeshRenderer>(id)->OnImgui();
		if (coord.HasComponent<ParticleEmitter>(id)) coord.GetComponent<ParticleEmitter>(id)->OnImgui();
		if (coord.HasComponent<Gizmo>(id)) coord.GetComponent<Gizmo>(id)->OnImgui();

		if (ImGui::BeginCombo("Add Component", ""))
		{
			if (!coord.HasComponent<RigidBody>(id) && ImGui::Selectable("RigidBody")) coord.AddComponent<RigidBody>(id);
			if (!coord.HasComponent<MeshRenderer>(id) && ImGui::Selectable("MeshRenderer")) coord.AddComponent<MeshRenderer>(id);
			if (!coord.HasComponent<ParticleEmitter>(id) && ImGui::Selectable("ParticleEmitter")) coord.AddComponent<ParticleEmitter>(id);
			if (!coord.HasComponent<Gizmo>(id) && ImGui::Selectable("Gizmo")) coord.AddComponent<Gizmo>(id);

			ImGui::EndCombo();
		}
	}
}