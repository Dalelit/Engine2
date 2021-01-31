#include "pch.h"
#include "RigidBody.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"

namespace Engine2
{
	using namespace DirectX;

	void RigidBody::OnUpdate(float dt, Transform* pTransform)
	{
	}

	void RigidBody::OnImgui()
	{
		ImGui::DragFloat("Mass", &mass, 0.1f);
		ImGui::Checkbox("IsKinematic", &isKinematic);
		ImGui::Checkbox("Use gravity", &useGravity);
	}

}