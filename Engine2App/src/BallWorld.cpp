#include "pch.h"
#include "BallWorld.h"
#include "MeshPrimatives.h"

using namespace Engine2;
using namespace DirectX;

BallWorld::BallWorld() : Layer("BallWorld")
{
	Engine::Get().mainCamera.SetPosition(2.0f, 3.0f, -4.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	CreateScene();
}

void BallWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void BallWorld::OnRender()
{
	scene.OnRender();
	pModel->OnRender();
}

void BallWorld::OnImgui()
{
	ImGui::Checkbox("Active", &active);
	pModel->OnImgui();
}

void BallWorld::CreateScene()
{
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 color;
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Normal", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Color", DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT},
	};

	std::vector<Vertex> verticies = {
		{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
		{ {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
		{ {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
		{ {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
		{ {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.8f, 0.1f, 1.0f} },
		{ {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.8f, 0.1f, 1.0f} },
		{ {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.8f, 0.1f, 1.0f} },
		{ {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.8f, 0.1f, 1.0f} },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f, 0.8f, 1.0f} },
		{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
		{ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.8f, 0.1f, 1.0f} },
		{ {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.8f, 0.1f, 1.0f} },
		{ {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.8f, 0.1f, 1.0f} },
		{ {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.8f, 0.1f, 1.0f} },
		{ {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
		{ {0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
		{ {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
	};
	std::vector<unsigned int> indicies = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23,
	};

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld1VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<Model>("Cube");
	model->pMesh = std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, indicies);
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	model->pMaterial->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));
	model->entities.instances.emplace_back(0.0f, 0.0f, 0.0f);

	pModel = model;
}
