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
	BoxWorld();
	~BoxWorld() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnInputEvent(Engine2::InputEvent& event) {};
	void OnApplicationEvent(Engine2::ApplicationEvent& event) {};
	void OnImgui();

	void CreateScene();
	void GSTestScene();
	void CreateScreenCopy();

protected:
	std::vector<std::shared_ptr<Engine2::Model>> models;
	Engine2::Scene scene;

	std::unique_ptr<Voxel> pVoxel;
	
	unsigned int offscreenId;
	std::vector<std::shared_ptr<Engine2::Bindable>> offscreenResources;
	std::shared_ptr<Engine2::Drawable> offscreenDrawable = nullptr;
	std::shared_ptr<Engine2::Texture> offscreenTexture = nullptr;

	struct GSConstantData
	{
		DirectX::XMMATRIX cameraTransform;
		DirectX::XMMATRIX entityTransform;
		DirectX::XMMATRIX entityTransformRotation;
	};

	std::shared_ptr<Engine2::Model> pGSTestModel = nullptr;
	std::shared_ptr<Engine2::GSConstantBuffer<GSConstantData>> pGSCB = nullptr;
};

