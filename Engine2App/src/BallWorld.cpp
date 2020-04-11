#include "pch.h"
#include "BallWorld.h"
#include "Primatives.h"

using namespace Engine2;
using namespace DirectX;

BallWorld::BallWorld() : Layer("BallWorld")
{
	Engine::Get().mainCamera.SetPosition(2.0f, 3.0f, -4.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 0.1f, 0.1f, 0.1f, 1.0f };
	scene.pointLights.emplace_back(PointLight({ -1.0f, 4.0f, -2.0f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }));

	CreateCube();
	CreateSphere();
}

void BallWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void BallWorld::OnRender()
{
	gizmos.NewFrame();

	scene.OnRender();

	gizmos.DrawAxis({ 0.0f, 0.0f, 0.0f, 1.0f });

	gizmos.DrawSphere(scene.pointLights[0].GetPosition());

	for (auto& m : models) if (m->IsActive()) m->OnRender();

	gizmos.Draw();
}

void BallWorld::OnImgui()
{
	ImGui::Checkbox("Active", &active);
	scene.OnImgui();
	gizmos.OnImgui();
	for (auto& m : models) m->OnImgui();
}

void BallWorld::CreateCube()
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

	std::vector<Vertex> verticies(Primatives::Cube::verticies.size());
	Primatives::CopyPositionNormalColor(verticies, Primatives::Cube::verticies);

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld1VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<Model>("Cube");
	model->pMesh = std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, Primatives::Cube::indicies);
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	model->pMaterial->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));
	model->entities.instances.emplace_back(1.0f, 0.0f, 0.0f);

	models.push_back(model);
}

void BallWorld::CreateSphere()
{
	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Normal", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Color", DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT},
	};

	auto sphereData = Primatives::IcoSphere::CreateIcoSphere(2);
	for (auto& v : *sphereData.verticies) v.color = { 0.1f, 0.2f, 0.8f, 1.0f };

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld1VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<Model>("IcoSphere");
	model->pMesh = std::make_shared<MeshTriangleIndexList<Primatives::PrimativesVertex>>(*sphereData.verticies, *sphereData.indicies);
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	model->pMaterial->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));
	model->entities.instances.emplace_back(-1.0f, 0.0f, 0.0f);

	models.push_back(model);
}
