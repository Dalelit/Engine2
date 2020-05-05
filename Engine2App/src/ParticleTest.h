#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Particles.h"

class ParticleTest : public Engine2::Layer
{
public:
	ParticleTest();
	~ParticleTest();

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();


protected:
	Engine2::Scene scene;

	Engine2::ParticleEmitter emitter;
};
