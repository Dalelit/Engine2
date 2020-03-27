#include "pch.h"
#include "Camera.h"
#include "Common.h"


namespace Engine2
{
	using namespace DirectX;

	void Camera::OnUpdate(float dt)
	{
		// move position if keys down

		RecalcViewMatrix();
		RecalcProjectionMatrix();
		RecalcViewProjectionMatrix();
	}

	// on mouse move rotate and clamp/wrap rotation
	bool Camera::OnMouseMove(MouseMoveEvent& event)
	{
		yaw += (float)event.GetX() * yawSpeed;
		WrapYaw();

		pitch += (float)event.GetY() * pitchSpeed;
		ClampPitch();

		return false;
	}

	// on window size change update aspect ratio
	bool Camera::OnWindowResize(WindowResizeEvent& event)
	{
		SetAspectRatio((float)event.GetWidth(), (float)event.GetHeight());
		return false;
	}

	void Camera::RecalcViewMatrix()
	{
		constexpr XMVECTOR forward = { 0.0f, 0.0f, 1.0f, 1.0f };
		constexpr XMVECTOR up = { 0.0f, 1.0f, 0.0f, 1.0f };

		XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
		XMVECTOR dir = XMVector3Normalize(XMVector3Transform(forward, rot));

		viewMatrix = XMMatrixLookToLH(position, dir, up);
	}

	void Camera::RecalcProjectionMatrix()
	{
		projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
	}

	void Camera::RecalcViewProjectionMatrix()
	{
		viewProjectionMatrix = viewMatrix * projectionMatrix;
	}

	void Camera::OnImugui()
	{
		if (ImGui::Begin("Camera"))
		{
			ImGui::DragFloat3("Position", position.m128_f32, 0.1f);
			ImGui::DragFloat("Yaw", &yaw, 0.01f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(yaw)); // note: not wrapping the yaw
			ImGui::DragFloat("Pitch", &pitch, 0.01f, -pitchBound, pitchBound); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(pitch));
			ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.25f, 0.1f);
			ImGui::DragFloat("FOV", &fov, 0.25f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(fov));
			ImGui::DragFloat("Near Z", &nearZ, 0.25f);
			ImGui::DragFloat("Far Z", &farZ, 0.25f);
			ImGui::DragFloat("YawSpeed", &yawSpeed);
			ImGui::DragFloat("PitchSpeed", &pitchSpeed);
			ImGui::End();
		}
	}

}