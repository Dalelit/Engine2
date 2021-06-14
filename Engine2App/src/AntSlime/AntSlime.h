#pragma once

#include "Layer.h"
#include "Scene.h"
#include "Shader.h"
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
	int antTypes = 3;
	void Initialise(int startPattern = 0);

	Engine2::DXBuffer ants1;
	Engine2::ComputeShaderUAV antuav1;

	Engine2::ComputeShader computeShader;
	Engine2::ComputeShader computeShaderAnts;

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
		int32_t senseRadius = 1;
		float replusion = 0.1f;
		float speed = 50.0f;
		float steeringStrength = 50.0f;
		float boundaryRange = 10.0f;
		float time;
		float deltaTime;
		float padding[2];
	};

	Engine2::CSConstantBuffer<ControlInfo> controlBuffer;
};
