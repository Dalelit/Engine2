#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Model.h"
#include "AssetLoaders/ObjLoader.h"
#include "Offscreen.h"
#include "Shader.h"

class ModelTest : public Engine2::Layer
{
public:
	ModelTest();
	~ModelTest();

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();
	void OnGizmos();


protected:
	Engine2::Scene scene;

	std::vector<Engine2::Model*> models;
	Engine2::Model* pSelectedModel = nullptr;

	Engine2::Offscreen stencil;
	void Outline();
	void CreateStencil();

	wrl::ComPtr<ID3D11DepthStencilState> pDSSWrite;
	wrl::ComPtr<ID3D11DepthStencilState> pDSSMask;
	wrl::ComPtr<ID3D11DepthStencilView> pDSVStencil;
	wrl::ComPtr<ID3D11Texture2D> pDTStencil;
	wrl::ComPtr<ID3D11BlendState> pBlendState;
	
	float outlineScale = 1.1f;
	Engine2::VSConstantBuffer<DirectX::XMVECTOR> vsOutlineCB;
	std::shared_ptr<Engine2::VertexShader> pVSOutline;

	DirectX::XMVECTOR outlineColor = { 0.8f, 0.8f, 0.2f, 0.75f};
	Engine2::PSConstantBuffer<DirectX::XMVECTOR> psOutlineCB;
	std::shared_ptr<Engine2::PixelShader> pPSOutline;
};
