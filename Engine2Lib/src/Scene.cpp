#include "pch.h"
#include "Scene.h"
#include "Camera.h"

namespace Engine2
{
	Scene::Scene() : vsConstBuffer(0), psConstBuffer(0)
	{
	}

	void Scene::OnRender()
	{
		UpdateVSConstBuffer();
		UpdatePSConstBuffer();
	}

	void Scene::OnImgui()
	{
		if (ImGui::TreeNode("Scene"))
		{
			if (ImGui::CollapsingHeader("Lights"))
			{
				ImGui::ColorEdit4("Ambient", psConstBuffer.data.ambientLight.m128_f32);

				for (auto& l : pointLights) l.OnImgui();
			}
			ImGui::TreePop();
		}
	}

	void Scene::UpdateVSConstBuffer()
	{
		Engine::Get().mainCamera.LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::UpdatePSConstBuffer()
	{
		psConstBuffer.data.CameraPosition = Engine::Get().mainCamera.GetPosition();
		if (pointLights.size() > 0)
		{
			psConstBuffer.data.pointLightPosition = pointLights[0].GetPosition();
			psConstBuffer.data.pointLightColor = pointLights[0].GetColor();
		}
		psConstBuffer.Bind();
	}
}