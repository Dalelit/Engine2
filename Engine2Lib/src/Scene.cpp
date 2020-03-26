#include "pch.h"
#include "Scene.h"

namespace Engine2
{
	void Scene::OnUpdate(float dt)
	{
		mainCamera.OnUpdate(dt);
	}

	void Scene::OnRender()
	{
		vsConstBuffer.data.cameraTransform = DirectX::XMMatrixTranspose(mainCamera.GetViewProjectionMatrix());
		vsConstBuffer.Bind();
	}
	void Scene::OnImgui()
	{
		static bool cameraOpen = true;
		if (cameraOpen) mainCamera.OnImugui();

	}
}