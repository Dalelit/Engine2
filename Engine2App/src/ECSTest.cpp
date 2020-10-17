#include "pch.h"
#include "ECSTest.h"
#include "Primatives.h"
#include "Engine2.h"
#include "VertexBuffer.h"
#include "Components.h"

using namespace Engine2;
using namespace EngineECS;
using namespace DirectX;

ECSTest::ECSTest() : Layer("ECSTest")
{
	Engine::GetActiveCamera().SetPosition(5.0f, 5.0f, -5.0f);
	Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	//scene.pointLights.emplace_back(PointLight({ -1.0f, 4.0f, -2.0f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }));

	CreateCube();

	auto e = scene.CreateEntity();
	auto pe = e.AddComponent<ParticleEmitter>();
	pe->SetMaxParticles(2000);
	pe->SetRate(500.0f);
}

void ECSTest::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void ECSTest::OnRender()
{
	scene.OnRender();
}

void ECSTest::OnImgui()
{
	scene.OnImgui();
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

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "StandardPosNorColVS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "StandardPosNorColPS.hlsl";

	auto e = scene.CreateEntity();
	e.GetComponent<Transform>()->Set(-3.0f, 0.0f, 0.0f);
	auto mesh = e.AddComponent<Mesh>();
	mesh->drawable = std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, Primatives::Cube::indicies);
	mesh->vertexShaderCB = std::make_shared<VSConstantBuffer<Transform>>(1);
	mesh->vertexShader = std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout);
	mesh->pixelShader = std::make_shared<PixelShaderDynamic>(psfilename);

	auto e2 = scene.CreateEntity();
	*e2.AddComponent<Mesh>() = *mesh;
	e2.GetComponent<Transform>()->Set(3.0f, 0.0f, 0.0f, 1.5f, 1.5f, 1.5f, 15.0f, 30.0f, 45.0f);
}
