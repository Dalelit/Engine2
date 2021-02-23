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
		
		if (orthographic)
			projectionMatrix = XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		else
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

			ImGui::Checkbox("Orthographic", &orthographic);

			if (ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.05f))
			{
				// call set aspect ratio as some calcs are triggered
				if (aspectRatio < 0.01f) SetAspectRatio(0.01f); // clamp it.
				else SetAspectRatio(aspectRatio);
			}
			ImGui::DragFloat("Near Z", &nearZ, 0.25f, 0.001f, farZ);
			ImGui::DragFloat("Far Z", &farZ, 0.25f, nearZ + 0.01f, 1000000.0f);

			if (orthographic)
			{
				// lock the width/height with the aspect ratio
				if (ImGui::DragFloat("View width", &viewWidth))
				{
					viewWidth = std::max<float>(1.0f, viewWidth);
					viewHeight = viewWidth / aspectRatio;
				}
				if (ImGui::DragFloat("View height", &viewHeight))
				{
					viewHeight = std::max<float>(1.0f, viewHeight);
					viewWidth = viewHeight * aspectRatio;
				}
			}
			else
			{
				float deg = XMConvertToDegrees(fov);
				if (ImGui::DragFloat("FOV", &deg, 0.1f)) fov = XMConvertToRadians(deg);
			}
			ImGui::TreePop();
		}
	}

	void Camera::Serialise(Serialisation::INode& node)
	{
		node.Attribute("name", name);
		node.Attribute("orthographic", orthographic);
		node.Attribute("aspectRatio", aspectRatio);
		node.Attribute("fov", fov);
		node.Attribute("nearZ", nearZ);
		node.Attribute("farZ", farZ);
		node.Attribute("aspectRatioLockedToScreen", aspectRatioLockedToScreen);
		node.Attribute("viewWidth", viewWidth);
		node.Attribute("viewHeight", viewHeight);
	}

	std::array<DirectX::XMVECTOR, 8> Camera::GetFrustrumPoints() const
	{
		if (orthographic)
			return GetFrustrumPointsOrthographic();
		else
			return GetFrustrumPointsPerspective();
	}

	DirectX::XMVECTOR Camera::GetFrustrumCentre() const
	{
		float distance = ((farZ - nearZ) * 0.5f) + nearZ;
		return FORWARD * distance;
	}

	std::array<DirectX::XMVECTOR, 8> Camera::GetFrustrumPointsOrthographic() const
	{
		float ydist = viewHeight * 0.5f;
		float xdist = viewWidth * 0.5f;

		return {
			XMVECTOR({-xdist,  ydist, nearZ, 1.0f}),
			XMVECTOR({ xdist,  ydist, nearZ, 1.0f}),
			XMVECTOR({ xdist, -ydist, nearZ, 1.0f}),
			XMVECTOR({-xdist, -ydist, nearZ, 1.0f}),
			XMVECTOR({-xdist, -ydist, farZ,  1.0f}),
			XMVECTOR({-xdist,  ydist, farZ,  1.0f}),
			XMVECTOR({ xdist,  ydist, farZ,  1.0f}),
			XMVECTOR({ xdist, -ydist, farZ,  1.0f}),
		};
	}

	std::array<DirectX::XMVECTOR, 8> Camera::GetFrustrumPointsPerspective() const
	{
		float ydist = tanf(fov / 2.0f);
		float xdist = ydist * aspectRatio;

		float nydist = ydist * nearZ;
		float nxdist = xdist * nearZ;
		float fydist = ydist * farZ;
		float fxdist = xdist * farZ;

		return {
			XMVECTOR({ -nxdist,  nydist, nearZ, 1.0f }),
			XMVECTOR({  nxdist,  nydist, nearZ, 1.0f }),
			XMVECTOR({  nxdist, -nydist, nearZ, 1.0f }),
			XMVECTOR({ -nxdist, -nydist, nearZ, 1.0f }),
			XMVECTOR({ -fxdist,  fydist, farZ,  1.0f }),
			XMVECTOR({  fxdist,  fydist, farZ,  1.0f }),
			XMVECTOR({  fxdist, -fydist, farZ,  1.0f }),
			XMVECTOR({ -fxdist, -fydist, farZ,  1.0f }),
		};
	}

	std::array<DirectX::XMVECTOR, 8> WorldCamera::GetFrustrumPoints()
	{
		auto points = camera.GetFrustrumPoints();

		std::for_each(points.begin(), points.end(), [&](XMVECTOR& p) { p = XMVector3Transform(p, worldTransform); });

		return points;
	}

	DirectX::XMVECTOR WorldCamera::GetFrustrumCentre()
	{
		return  XMVector3Transform(camera.GetFrustrumCentre(), worldTransform);
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