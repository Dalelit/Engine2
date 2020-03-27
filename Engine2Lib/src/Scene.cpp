#include "pch.h"
#include "Scene.h"
#include "Camera.h"

namespace Engine2
{
	void Scene::OnUpdate(float dt)
	{
	}

	void Scene::OnRender()
	{
		Engine::Get().mainCamera.LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::OnImgui()
	{
	}
}