#include "pch.h"
#include "Components.h"

using namespace DirectX;

namespace Engine2
{
	void EntityInfo::OnImgui()
	{
		char buffer[256];
		strcpy_s(buffer, 256, tag.c_str());
		if (ImGui::InputText("Tag", buffer, 256))
		{
			//tag = buffer;
		}
	}

	void Transform::Set(float positionX, float positionY, float positionZ, float scaleX, float scaleY, float scaleZ, float rollDeg, float pitchDeg, float yawDeg)
	{
		rotation.roll = rollDeg;
		rotation.pitch = pitchDeg;
		rotation.yaw = yawDeg;
		
		transform = XMMatrixScaling(scaleX, scaleY, scaleZ);
		transform *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.pitch), XMConvertToRadians(rotation.yaw), XMConvertToRadians(rotation.roll));
		transform *= XMMatrixTranslation(positionX, positionY, positionZ);
		transform = XMMatrixTranspose(transform);
	}

	void Transform::OnImgui()
	{
		// to do:
		// - is there a way to get human readable rotation out of the transform matrix?
		// - scale going negative gets messed up... assuming it's to do with the decompose?
		if (ImGui::TreeNode("Transform"))
		{
			XMVECTOR vScale, vRotQuat, vTrans;
			XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, XMMatrixTranspose(transform));

			auto MatrixCompose = [&]() {
				//transform *= XMMatrixRotationQuaternion(vRotQuat);
				transform = XMMatrixScalingFromVector(vScale);
				transform *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.pitch), XMConvertToRadians(rotation.yaw), XMConvertToRadians(rotation.roll));
				transform *= XMMatrixTranslationFromVector(vTrans);
				transform = XMMatrixTranspose(transform);
			};

			if (ImGui::DragFloat3("Position", vTrans.m128_f32, 0.1f)) MatrixCompose();
			if (ImGui::DragFloat3("Scale", vScale.m128_f32, 0.1f))    MatrixCompose();
			if (ImGui::DragFloat3("Roll/Pitch/Yaw", &rotation.roll, 1.0f)) MatrixCompose();
			
			if (ImGui::Button("Reset"))
			{
				transform = XMMatrixIdentity();
				rotation.roll = rotation.pitch = rotation.yaw = 0.0f;
			}

			ImGui::TreePop();
		}
	}
	
	void Mesh::OnImgui()
	{
		if (ImGui::TreeNode("Mesh"))
		{
			if (drawable) drawable->OnImgui();
			else ImGui::Text("Drawable null");

			if (vertexShaderCB) vertexShaderCB->OnImgui();
			else ImGui::Text("vertexShaderCB null");

			if (vertexShader) vertexShader->OnImgui();
			else ImGui::Text("vertexShader null");

			if (pixelShader) pixelShader->OnImgui();
			else ImGui::Text("pixelShader null");

			ImGui::TreePop();
		}
	}

	void Gizmo::OnImgui()
	{
		if (ImGui::TreeNode("Gizmo"))
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
		if (coord.HasComponent<Mesh>(id)) coord.GetComponent<Mesh>(id)->OnImgui();
		if (coord.HasComponent<ParticleEmitter>(id)) coord.GetComponent<ParticleEmitter>(id)->OnImgui();
		if (coord.HasComponent<Gizmo>(id)) coord.GetComponent<Gizmo>(id)->OnImgui();

		if (ImGui::BeginCombo("Add Component", ""))
		{
			if (!coord.HasComponent<Mesh>(id) && ImGui::Selectable("Mesh")) coord.AddComponent<Mesh>(id);
			if (!coord.HasComponent<ParticleEmitter>(id) && ImGui::Selectable("ParticleEmitter")) {
				coord.AddComponent<ParticleEmitter>(id);
			}
			if (!coord.HasComponent<Gizmo>(id) && ImGui::Selectable("Gizmo")) coord.AddComponent<Gizmo>(id);

			ImGui::EndCombo();
		}
	}
}