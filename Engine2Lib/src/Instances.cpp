#include "pch.h"
#include "Instances.h"
#include "Common.h"

using namespace DirectX;

namespace Engine2
{
	unsigned long Instance::InstanceCounter = 0;

	Instance::Instance(XMVECTOR position,
		XMVECTOR rotation,
		XMVECTOR scale)
		: id(std::to_string(InstanceCounter++)),
		position(position),
		rotation(rotation),
		scale(scale)
	{
	};

	void Instance::LoadTransformT(DirectX::XMMATRIX& invTransform, DirectX::XMMATRIX& invRotationTransform)
	{
		invRotationTransform = XMMatrixScalingFromVector(scale) * XMMatrixRotationRollPitchYawFromVector(rotation);
		invTransform = invRotationTransform * XMMatrixTranslationFromVector(position);

		// transpose for directx
		invRotationTransform = XMMatrixTranspose(invRotationTransform);
		invTransform = XMMatrixTranspose(invTransform);
	}

	void Instance::OnImgui()
	{
		if (ImGui::TreeNode(id.c_str()))
		{
			// to do: change to float3?... leaving for now to make w visible
			ImGui::Checkbox("Active", &active);
			ImGui::DragFloat4("Position", position.m128_f32, 0.1f);
			ImGui::DragFloat4("Rotation", rotation.m128_f32, 0.1f);
			ImGui::DragFloat4("Scale", scale.m128_f32, 0.1f);
			ImGui::TreePop();
		}
	}
}
