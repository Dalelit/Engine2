#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"

class Empty : public Engine2::Layer
{
public:
	Empty();
	~Empty();

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();


protected:
	Engine2::Scene scene;
};
