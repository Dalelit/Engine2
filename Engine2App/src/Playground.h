#pragma once

#include "Common.h"
#include "Layer.h"
#include "Model.h"
#include "ConstantBuffer.h"
#include "Scene.h"

class Playground : public Engine2::Layer
{
public:

	Playground() : Layer("Playground") { CreateScene(); }

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();

	void CreateScene();


private:
	std::vector<std::shared_ptr<Engine2::Model>> models;
	Engine2::Scene scene;

	void AddModel1();
	void AddModel2();
	void AddModel3();
	void AddModel4();
};
