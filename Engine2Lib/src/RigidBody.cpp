#include "pch.h"
#include "RigidBody.h"
#include "submodules/imgui/imgui.h"
#include "UtilMath.h"

namespace Engine2
{
	using namespace DirectX;

	XMVECTOR RigidBody::gravity = { 0.0f, -9.8f, 0.0f, 1.0f };

	void RigidBody::OnUpdate(float dt, Transform* pTransform)
	{
		//XMVECTOR force = gravity;
		//XMVECTOR acceleration = (force / mass) * dt;
		//velocity += acceleration;

		velocity += (gravity / mass) * dt;
		//velocity = XMVectorSetW(velocity, 1.0f);
		//XMMATRIX m = XMMatrixTranslationFromVector(velocity * dt);
		XMMATRIX m = Math::TransformMatrixEuler(XMVectorSetW(velocity * dt, 1.0f), angularVelocity * dt);

		//pTransform->transform *= XMMatrixTranspose(m);
		pTransform->transform = XMMatrixTranspose(m) * pTransform->transform;
	}

	void RigidBody::OnImgui()
	{
		if (ImGui::TreeNodeEx("RigidBody", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Velocity", velocity.m128_f32, 0.1f);
			ImGui::DragFloat3("Angular Velocity", angularVelocity.m128_f32, 0.1f);
			ImGui::DragFloat("Mass", &mass, 0.1f);
			ImGui::DragFloat3("Common gravity", gravity.m128_f32, 0.1f);

			if (ImGui::Button("Reset"))
			{
				velocity = g_XMZero;
				angularVelocity = g_XMZero;
			}

			ImGui::TreePop();
		}
	}

}