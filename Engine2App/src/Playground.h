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
	bool OnInputEvent(Engine2::InputEvent& event);
	bool OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnImgui();

	void CreateScene();


private:
	std::vector<std::shared_ptr<Engine2::Model>> models;
	Engine2::Scene scene;

	bool OnMouseMove(Engine2::MouseMoveEvent& event);

	void AddModel1();
	void AddModel2();
	void AddModel3();
	void AddModel4();
};
