#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Model.h"

class BallWorld : public Engine2::Layer
{
public:
	BallWorld();
	~BallWorld() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();

protected:
	Engine2::Scene scene;

	std::vector<std::shared_ptr<Engine2::Model>> models;

	void CreateCube();
	void CreateSphere();
	void CreateAxis();
};

