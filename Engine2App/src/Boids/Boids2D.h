#pragma once

#include "Layer.h"
#include "ComputeShader.h"
#include "ComputeShaderBuffer.h"
#include "ConstantBuffer.h"
#include "Offscreen.h"
#include "InputController.h"
#include "VertexBuffer.h"

class Boids2D : public Engine2::Layer
{
public:
	Boids2D();
	~Boids2D() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnInputEvent(Engine2::InputEvent& event);
	void OnImgui();

protected:
	int state = 1;
	uint32_t boidCount = 10;

	Engine2::InputController input;

	//--------------------------------------------------------
	void InitialiseGfx();

	Engine2::VertexBufferIndex pVB;
	std::shared_ptr<Engine2::VertexShader> pVS;
	std::shared_ptr<Engine2::PixelShader>  pPS;

	//--------------------------------------------------------
	void InitialiseCS(int startPattern = 0);

	std::unique_ptr<Engine2::Offscreen> tgt1;
	std::unique_ptr<Engine2::Offscreen> tgt2;

	Engine2::DXBuffer boidBuffer;
	UINT boidSlot = 1;
	wrl::ComPtr<ID3D11ShaderResourceView> pBoidVSSRV = nullptr;

	std::shared_ptr<Engine2::ComputeShaderFile> pCSTrails;
	std::shared_ptr<Engine2::ComputeShaderFile> pCSBoids;

	Engine2::ComputeShaderSRV texsrv1;
	Engine2::ComputeShaderSRV texsrv2;

	Engine2::ComputeShaderUAV texuav1;
	Engine2::ComputeShaderUAV texuav2;
	Engine2::ComputeShaderUAV boiduav;

	struct ControlInfo
	{
		float   time;
		float   deltaTime;
		int32_t xmax;
		int32_t ymax;
		int32_t diffuseRadius = 2;
		float   diffuseRate = 0.2f;
		float   diffuseFade = 0.01f;
		int32_t xMouse = -1;
		int32_t yMouse = -1;
		int32_t boidCount;
		float   padding[2];
	};

	Engine2::CSConstantBuffer<ControlInfo> controlBuffer;
};
