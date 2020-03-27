#include "pch.h"
#include "Camera.h"
#include "Common.h"


namespace Engine2
{
	using namespace DirectX;

	void Camera::LoadViewProjectionMatrixT(DirectX::XMMATRIX& vpMatrix)
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;

		// view matrix
		constexpr XMVECTOR forward = { 0.0f, 0.0f, 1.0f, 1.0f };
		constexpr XMVECTOR up = { 0.0f, 1.0f, 0.0f, 1.0f };

		XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
		XMVECTOR dir = XMVector3Normalize(XMVector3Transform(forward, rot));

		viewMatrix = XMMatrixLookToLH(position, dir, up);

		// projection matrix
		projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

		// view projection matrix
		vpMatrix = XMMatrixTranspose(viewMatrix * projectionMatrix); // transpose for DX.
	}

	void Camera::ImuguiWindow(bool* pOpen)
	{
		if (ImGui::Begin("Camera", pOpen))
		{
			ImGui::DragFloat3("Position", position.m128_f32, 0.1f);
			ImGui::DragFloat("Yaw", &yaw, 0.01f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(yaw)); // note: not wrapping the yaw
			ImGui::DragFloat("Pitch", &pitch, 0.01f, -pitchBound, pitchBound); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(pitch));
			ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.25f, 0.1f);
			ImGui::DragFloat("FOV", &fov, 0.25f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(fov));
			ImGui::DragFloat("Near Z", &nearZ, 0.25f);
			ImGui::DragFloat("Far Z", &farZ, 0.25f);
		}
		ImGui::End();
	}

}