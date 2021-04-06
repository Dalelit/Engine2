#pragma once

#include "Layer.h"
#include "Scene.h"
#include "ComputeShader.h"
#include "ComputeShaderBuffer.h"
#include "Texture.h"
#include "TextureGizmo.h"

class AntSlime : public Engine2::Layer
{
public:
	AntSlime();
	~AntSlime() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnImgui();

protected:
	
	int antCount = 10000;
	void Initialise();

	Engine2::DXBuffer ants1;
	Engine2::ComputeShaderUAV antuav1;

	std::shared_ptr<Engine2::ComputeShaderFile> pComputeShader;
	std::shared_ptr<Engine2::ComputeShaderFile> pComputeShaderAnts;

	std::unique_ptr<Engine2::Texture> tex1;
	std::unique_ptr<Engine2::Texture> tex2;

	Engine2::ComputeShaderSRV texsrv1;
	Engine2::ComputeShaderSRV texsrv2;

	Engine2::ComputeShaderUAV texuav1;
	Engine2::ComputeShaderUAV texuav2;

	Engine2::TextureGizmo tex1Gizmo;
	Engine2::TextureGizmo tex2Gizmo;

	UINT state = 0;
	bool pause = false;

	struct ControlInfo
	{
		float xmax;
		float ymax;
		float locationRange = 1.0f;
		float diffuseLoss = 0.2f;
		float diffuseFade = 0.99f;
		float senseAngle = 30.0f / 360.0f * DirectX::XM_2PI;
		float senseRange = 6.0f;
		float speed = 50.0f;
		float steeringStrength = 50.0f;
		float boundaryForce = 10.0f;
		float boundaryRange = 10.0f;
		float time;
		float deltaTime;
		float padding[3];
	};

	Engine2::CSConstantBuffer<ControlInfo> controlBuffer;
};
