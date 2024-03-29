#pragma once

#include "Layer.h"
#include "Shader.h"
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
	uint32_t boidCount = 400;

	Engine2::InputController input;

	bool showSenseRadius = false;
	bool showSenseLines  = false;

	//--------------------------------------------------------
	void InitialiseGfx();

	Engine2::VertexBufferIndex vb;
	Engine2::VertexShader vs;
	Engine2::PixelShader  ps;

	Engine2::VertexBufferIndex vbSense;
	Engine2::VertexShader vsSense;
	Engine2::PixelShader  psSense;

	Engine2::VertexBuffer   vbSenseLines;
	Engine2::VertexShader   vsSenseLines;
	Engine2::PixelShader    psSenseLines;
	Engine2::GeometryShader gsSenseLines;

	struct VSCBData
	{
		float worldHeight = 100.0f;
		float worldWidth;
		float padding[2];
	};

	//--------------------------------------------------------
	void InitialiseCS(int startPattern = 0);

	std::unique_ptr<Engine2::Offscreen> tgt1;
	std::unique_ptr<Engine2::Offscreen> tgt2;

	Engine2::DXBuffer boidBuffer;
	UINT boidSlot = 1;
	wrl::ComPtr<ID3D11ShaderResourceView> pBoidVSSRV = nullptr;

	Engine2::ComputeShader csTrails;
	Engine2::ComputeShader csBoids;

	Engine2::ComputeShaderSRV texsrv1;
	Engine2::ComputeShaderSRV texsrv2;

	Engine2::ComputeShaderUAV texuav1;
	Engine2::ComputeShaderUAV texuav2;
	Engine2::ComputeShaderUAV boiduav;

	struct WorldInfo
	{
		DirectX::XMFLOAT2 worldDimension = {100.0f, 80.0f};
		DirectX::XMUINT2  screenDimension;
		//float   padding[2];
	};

	struct ControlInfo
	{
		float   time;
		float   deltaTime;
		int32_t diffuseRadius = 2;
		float   diffuseRate = 0.2f;
		float   diffuseFade = 0.01f;
		int32_t boidCount;
		float   boidSpeed = 25.0f;
		float   boidScale = 1.0f;
		float   boidSenseRadius = 10.0f;
		float   centreStrength = 0.9f;
		float   directionStrength = 0.9f;
		float   repulsionStrength = 0.2f;
		//float   padding[];
	};

	Engine2::ConstantBuffer<WorldInfo> worldCB;
	Engine2::CSConstantBuffer<ControlInfo> controlCB;

	inline void CalcWorldWidth() { worldCB.data.worldDimension.x = worldCB.data.worldDimension.y * Engine2::DXDevice::Get().GetAspectRatio(); }
};
