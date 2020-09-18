#pragma once

#include "Layer.h"
#include "Scene.h"
#include "Model.h"
#include "ECS.h"

class ECSTest : public Engine2::Layer
{
public:
	ECSTest();
	~ECSTest() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();

protected:
	Engine2::Scene scene;

	std::vector<std::shared_ptr<Engine2::Model>> models;

	EngineECS::Coordinator coordinator;

	void CreateCube();
};
