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
		pTransform->position += velocity * dt;
		pTransform->rotation += angularVelocity * dt;
	}

	void RigidBody::OnImgui()
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
	}

}