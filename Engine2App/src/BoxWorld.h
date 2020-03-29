#pragma once
#include "Common.h"
#include "Layer.h"
#include "Model.h"
#include "ConstantBuffer.h"
#include "Scene.h"
#include "Voxel.h"

class BoxWorld : public Engine2::Layer
{
public:
	BoxWorld() : Layer("BoxWorld") { CreateScene(); }

	void OnUpdate(float dt);
	void OnRender();
	void OnInputEvent(Engine2::InputEvent& event) {};
	void OnApplicationEvent(Engine2::ApplicationEvent& event) {};
	void OnImgui();

	void CreateScene();

protected:
	std::vector<std::shared_ptr<Engine2::Model>> models;
	Engine2::Scene scene;

	std::unique_ptr<Voxel> pVoxel;
};

