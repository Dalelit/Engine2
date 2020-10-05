#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Model.h"
#include "AssetLoaders/ObjLoader.h"
#include "OffscreenOutliner.h"
#include "Shader.h"

class ModelTest : public Engine2::Layer
{
public:
	ModelTest();
	~ModelTest();

	void OnUpdate(float dt);
	void OnRender();
	void OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnImgui();

protected:
	Engine2::Scene scene;

	std::vector<Engine2::Model*> models;
	Engine2::Model* pSelectedModel = nullptr;

	Engine2::OffscreenOutliner outliner;
	void Outline();
};
