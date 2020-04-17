#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Model.h"
#include "AssetLoaders/ObjLoader.h"

class ModelTest : public Engine2::Layer
{
public:
	ModelTest();
	~ModelTest();

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();

protected:
	Engine2::Scene scene;

	std::vector<Engine2::Model*> models;

	void AddModel(std::shared_ptr<Engine2::AssetLoaders::ObjLoader>& loadedModel, std::string objectName, Engine2::Model* pModel);
};

