#include "pch.h"
#include "Transform.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"

namespace Engine2
{
	using namespace DirectX;

	DirectX::XMMATRIX Transform::Matrix()
	{
		return XMMatrixScalingFromVector(scale) * XMMatrixRotationRollPitchYawFromVector(rotation) * XMMatrixTranslationFromVector(position);
	}

	DirectX::XMVECTOR Transform::Forward()
	{
		auto qrot = XMQuaternionRotationRollPitchYawFromVector(rotation);

		return XMVector3Rotate(Directions.forward, qrot);
	}

	void Transform::LookAt(DirectX::XMVECTOR location)
	{
		XMVECTOR lookAtDir = XMVector3Normalize(location - position);

		rotation = {
			-asinf(lookAtDir.m128_f32[1]),
			atan2f(lookAtDir.m128_f32[0], lookAtDir.m128_f32[2]),
			0.0f,
			0.0f
		};
		
	}

	void Transform::Move(float forward)
	{
		auto qrot = XMQuaternionRotationRollPitchYawFromVector(rotation);

		position += XMVector3Rotate(Directions.forward, qrot) * forward;
	}

	void Transform::Move(float forward, float right, float up)
	{
		auto qrot = XMQuaternionRotationRollPitchYawFromVector(rotation);
		
		XMVECTOR translation = XMVector3Rotate(Directions.forward, qrot) * forward;
		translation += XMVector3Rotate(Directions.right, qrot) * right;
		translation += XMVector3Rotate(Directions.up, qrot) * up;

		position += translation;
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

	TransformMatrix::TransformMatrix(Transform& transform) :
		rotationMatrix(XMMatrixRotationRollPitchYawFromVector(transform.rotation)),
		transformMatrix(XMMatrixScalingFromVector(transform.scale)* rotationMatrix* XMMatrixTranslationFromVector(transform.position))
	{
		// same as set
	}

	TransformMatrix::TransformMatrix(XMMATRIX transformMatrix, XMMATRIX rotationMatrix) :
		transformMatrix(transformMatrix), rotationMatrix(rotationMatrix)
	{
	}

	void TransformMatrix::Set(Transform& transform)
	{
		rotationMatrix = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
		transformMatrix = XMMatrixScalingFromVector(transform.scale) * rotationMatrix * XMMatrixTranslationFromVector(transform.position);
	}

	void TransformMatrix::Set(DirectX::XMVECTOR position, DirectX::XMVECTOR quaterion, DirectX::XMVECTOR scale)
	{
		rotationMatrix = XMMatrixRotationQuaternion(quaterion);
		transformMatrix = XMMatrixScalingFromVector(scale) * rotationMatrix * XMMatrixTranslationFromVector(position);
	}

	void TransformMatrix::OnImgui()
	{
		if (ImGui::TreeNodeEx("TransformMatrix"))
		{
			XMVECTOR vScale, vRotQ, vTrans, vRotEulerDeg, vRotEulerDegNew;
			XMMatrixDecompose(&vScale, &vRotQ, &vTrans, transformMatrix);
			vRotEulerDegNew = vRotEulerDeg = Math::RadToDeg(Math::QuaternionToEuler(vRotQ));

			ImGui::DragFloat3("Position", vTrans.m128_f32, 0.1f);
			ImGui::DragFloat3("Scale", vScale.m128_f32, 0.1f);
			ImGui::DragFloat3("Roll/Pitch/Yaw", vRotEulerDegNew.m128_f32, 0.5f);

			ImGui::TreePop();
		}
	}
}
