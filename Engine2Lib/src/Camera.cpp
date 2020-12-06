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
		XMMATRIX transform = rot * XMMatrixTranslationFromVector(position);
		XMVECTOR direction = XMVector3Transform(FORWARD, rot); // rotate from the default forward direciton 0,0,1

		// note: XMMatrixLookToLH normalises the direction vector
		viewMatrix = XMMatrixLookToLH(position, direction, UP);
		projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
		viewProjectionMatrix = viewMatrix * projectionMatrix;
	}

	void Camera::OnImgui()
	{
		if (ImGui::TreeNode(this, name.c_str()))
		{
			char buffer[256];
			strcpy_s(buffer, sizeof(buffer), name.c_str());
			if (ImGui::InputText("Name", buffer, sizeof(buffer))) name = buffer;

			if (ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.05f)) if (aspectRatio < 0.01f) aspectRatio = 0.01f;
			float deg = XMConvertToDegrees(fov);
			if (ImGui::DragFloat("FOV", &deg, 0.1f)) fov = XMConvertToRadians(deg);
			ImGui::DragFloat("Near Z", &nearZ, 0.25f, 0.001f, farZ);
			ImGui::DragFloat("Far Z", &farZ, 0.25f, nearZ + 0.01f, 1000000.0f);
			ImGui::TreePop();
		}
	}

	//Ray Camera::ScreenCoordToRay(float x, float y)
	//{
	//	Ray ray;

	//	// get screen distances
	//	float ydist = tanf(fov / 2.0f) * nearZ;
	//	float xdist = ydist * aspectRatio;

	//	// move coorindates to be center of the screen
	//	x = x * 2.0f - 1.0f;
	//	y = y * 2.0f - 1.0f;

	//	// move origin at the centre of the screen
	//	ray.origin = position + nearZ * direction;

	//	// move to the x coordinate
	//	XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0.0f, 1.0f, 0.0f, 1.0f }, direction));
	//	ray.origin += right * x * xdist;

	//	// move to the y coordinate. Invert direction as NDC top left is 0,0
	//	XMVECTOR up = XMVector3Normalize(XMVector3Cross(direction, right));
	//	ray.origin -= up * y * ydist;

	//	// direction of ray is from position to the point
	//	ray.direction = XMVector3Normalize(ray.origin - position);

	//	return ray;
	//}
}