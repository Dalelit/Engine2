#include "pch.h"
#include "ECSTest.h"
#include "Primatives.h"
#include "Engine2.h"

using namespace Engine2;
using namespace DirectX;

ECSTest::ECSTest() : Layer("ECSTest")
{
	Engine::GetActiveCamera().SetPosition(5.0f, 5.0f, -5.0f);
	Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	//scene.pointLights.emplace_back(PointLight({ -1.0f, 4.0f, -2.0f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }));

	CreateCube();
}

void ECSTest::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void ECSTest::OnRender()
{
	scene.OnRender();

	for (auto& m : models) if (m->IsActive()) m->Draw();
}

void ECSTest::OnImgui()
{
	scene.OnImgui();
	for (auto& m : models) m->OnImgui();
}

void ECSTest::CreateCube()
{
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 color;
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal",      0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color",       0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	std::vector<Vertex> verticies(Primatives::Cube::verticies.size());
	Primatives::CopyPositionNormalColor(verticies, Primatives::Cube::verticies);

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "ModelTestVS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "ModelTestPS.hlsl";

	auto model = std::make_shared<ModelEntities>("Cube");
	auto rn = model->AddRenderNode("RN1");
	rn->SetDrawable(std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, Primatives::Cube::indicies));
	rn->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	rn->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));

	model->entities.instances.emplace_back(1.0f, 0.0f, 0.0f);

	models.push_back(model);
}
