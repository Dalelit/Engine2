#include "pch.h"
#include "Empty.h"
#include "Engine2.h"

using namespace Engine2;
using namespace DirectX;

Empty::Empty() : Layer("Empty")
{
	{
		auto mainCamera = scene.CreateSceneCamera("Main", true);
		auto t = mainCamera.GetComponent<Transform>();
		t->SetPosition(5.0f, 5.0f, -5.0f);
		t->LookAt(0.0f, 0.0f, 0.0f);
	}
}

Empty::~Empty()
{
}

void Empty::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void Empty::OnRender()
{
	scene.OnRender();
}

void Empty::OnImgui()
{
}
