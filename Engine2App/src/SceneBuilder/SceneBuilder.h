#pragma once

#include "Layer.h"
#include "Scene.h"

class SceneBuilder : public Engine2::Layer
{
public:
	SceneBuilder();
	~SceneBuilder() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnImgui();

protected:
	Engine2::Scene scene;

	bool onClearIncludeAssets = true;

	void CreateDefaultAssets();
	void CreateEmtpyScene();
	void BuildTestScene();
	void ClearScene();
};
