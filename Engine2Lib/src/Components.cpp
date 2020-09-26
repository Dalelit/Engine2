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
			drawable->OnImgui();
			vertexShader->OnImgui();
			pixelShader->OnImgui();
			ImGui::TreePop();
		}
	}
}