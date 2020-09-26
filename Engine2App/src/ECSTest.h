#pragma once

#include "Layer.h"
#include "Scene.h"

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

	void CreateCube();
};
