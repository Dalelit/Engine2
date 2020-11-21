#include "pch.h"
#include "Camera.h"
#include "Common.h"


namespace Engine2
{
	using namespace DirectX;

	void Camera::Update(DirectX::XMVECTOR position, DirectX::XMVECTOR rotation)
	{
		// view matrix
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);
		transform = rot * XMMatrixTranslationFromVector(position);
		direction = XMVector3Transform(FORWARD, rot); // rotate from the default forward direciton 0,0,1

		// note: XMMatrixLookToLH normalises the direction vector
		viewMatrix = XMMatrixLookToLH(position, direction, UP);
		projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
		viewProjectionMatrix = viewMatrix * projectionMatrix;
	}

	void Camera::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.25f, 0.1f, 0.1f);
			ImGui::DragFloat("FOV", &fov, 0.01f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(fov));
			ImGui::DragFloat("Near Z", &nearZ, 0.25f, 0.001f, farZ);
			ImGui::DragFloat("Far Z", &farZ, 0.25f, nearZ + 0.01f, 1000000.0f);
			ImGui::TreePop();
		}
	}

	void SceneCamera::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::DragFloat3("Position", position.m128_f32, 0.1f);
			ImGui::DragFloat("Yaw", &yaw, 0.01f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(yaw)); // note: not wrapping the yaw
			ImGui::DragFloat("Pitch", &pitch, 0.01f, -pitchBound, pitchBound); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(pitch));
			ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.25f, 0.1f, 0.1f);
			ImGui::DragFloat("FOV", &fov, 0.01f); ImGui::SameLine(); ImGui::Text("%.1f Degs", XMConvertToDegrees(fov));
			ImGui::DragFloat("Near Z", &nearZ, 0.25f, 0.001f, farZ);
			ImGui::DragFloat("Far Z", &farZ, 0.25f, nearZ + 0.01f, 1000000.0f);
			ImGui::TreePop();
		}
	}

	void SceneCamera::Update()
	{
		Camera::Update(position, {pitch, yaw, 0.0f, 0.0f });
	}

	void SceneCamera::Move(float forwardDist, float rightDist, float upDist)
	{
		position += forwardDist * direction;

		XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0.0f, 1.0f, 0.0f, 1.0f }, direction));
		position += right * rightDist;

		XMVECTOR up = XMVector3Normalize(XMVector3Cross(direction, right));
		position += up * upDist;
	}

	void SceneCamera::LookAt(float x, float y, float z)
	{
		XMVECTOR lookAtDir = XMVector3Normalize(XMVECTOR({ x, y, z, 1.0f }) - position);

		pitch = asinf(lookAtDir.m128_f32[1]);
		yaw = atan2f(lookAtDir.m128_f32[0], lookAtDir.m128_f32[2]);
	}

	Ray SceneCamera::ScreenCoordToRay(float x, float y)
	{
		Ray ray;

		// get screen distances
		float ydist = tanf(fov / 2.0f) * nearZ;
		float xdist = ydist * aspectRatio;

		// move coorindates to be center of the screen
		x = x * 2.0f - 1.0f;
		y = y * 2.0f - 1.0f;

		// move origin at the centre of the screen
		ray.origin = position + nearZ * direction;

		// move to the x coordinate
		XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0.0f, 1.0f, 0.0f, 1.0f }, direction));
		ray.origin += right * x * xdist;

		// move to the y coordinate. Invert direction as NDC top left is 0,0
		XMVECTOR up = XMVector3Normalize(XMVector3Cross(direction, right));
		ray.origin -= up * y * ydist;

		// direction of ray is from position to the point
		ray.direction = XMVector3Normalize(ray.origin - position);

		return ray;
	}

	Ray SceneCamera::ForwardDirectionRay()
	{
		return { position + nearZ * direction, direction };
	}
}