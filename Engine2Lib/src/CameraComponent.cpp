#include "pch.h"
#include "CameraComponent.h"

namespace Engine2
{
	CameraComponent::CameraComponent() : camera("Default")
	{
		pBuffer = std::make_unique<OffscreenWithDepthBuffer>();
		camera.SetAspectRatio(DXDevice::Get().GetAspectRatio());
	}

	void CameraComponent::OnImgui()
	{
		if (ImGui::TreeNode("Camera"))
		{
			camera.OnImgui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Buffer"))
		{
			if (pBuffer) pBuffer->OnImgui();
			ImGui::TreePop();
		}
	}
}