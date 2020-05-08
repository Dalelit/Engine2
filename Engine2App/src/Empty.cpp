#include "pch.h"
#include "Empty.h"
#include "Engine2.h"

using namespace Engine2;
using namespace DirectX;

Empty::Empty() : Layer("Empty")
{
	Engine::GetActiveCamera().SetPosition(0.0f, 2.0f, -3.0f);
	Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);
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
