#include "pch.h"
#include "ParticleTest.h"
#include "Engine2.h"

using namespace Engine2;
using namespace DirectX;

ParticleTest::ParticleTest() : Layer("ParticleTest")
{
	Engine::GetActiveCamera().SetPosition(0.0f, 2.0f, -3.0f);
	//Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);
}

ParticleTest::~ParticleTest()
{
}

void ParticleTest::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
	emitter.OnUpdate(dt);
}

void ParticleTest::OnRender()
{
	scene.OnRender();
	emitter.OnRender();
}

void ParticleTest::OnImgui()
{
	emitter.OnImgui();
}
