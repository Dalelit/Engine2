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
		XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
		forward = XMVector3Normalize(XMVector3Transform({ 0.0f, 0.0f, 1.0f, 1.0f }, rot)); // rotate from the default forward direciton 0,0,1

		viewMatrix = XMMatrixLookToLH(position, forward, { 0.0f, 1.0f, 0.0f, 1.0f }); // default up 0,1,0

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
			ImGui::DragFloat("FOV", &fov, 0.01f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(fov));
			ImGui::DragFloat("Near Z", &nearZ, 0.25f, 0.001f, farZ);
			ImGui::DragFloat("Far Z", &farZ, 0.25f, nearZ + 0.01f, 1000000.0f);
		}
		ImGui::End();
	}

	void Camera::Move(float forwardDist, float rightDist, float upDist)
	{
		position += forwardDist * forward;

		XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0.0f, 1.0f, 0.0f, 1.0f }, forward));
		position += right * rightDist;

		XMVECTOR up = XMVector3Normalize(XMVector3Cross(forward, right));
		position += up * upDist;
	}

}