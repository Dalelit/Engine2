#include "pch.h"
#include "BallWorld.h"
#include "Primatives.h"

using namespace Engine2;
using namespace DirectX;

BallWorld::BallWorld() : Layer("BallWorld")
{
	Engine::Get().mainCamera.SetPosition(2.0f, 3.0f, -4.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	CreateCube();
	CreateCubeWireframe();
	CreateAxis();
}

void BallWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void BallWorld::OnRender()
{
	scene.OnRender();

	for (auto& m : models) if (m->IsActive()) m->OnRender();
}

void BallWorld::OnImgui()
{
	ImGui::Checkbox("Active", &active);
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
	model->entities.instances.emplace_back(0.0f, 0.0f, 0.0f);

	models.push_back(model);
}

void BallWorld::CreateCubeWireframe()
{
	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
	};

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "WireframeBasicVS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "WireframeBasicPS.hlsl";

	auto model = std::make_shared<Model>("Cube Wireframe");
	model->pMesh = std::make_shared<WireframeIndexList<XMFLOAT3>>(Primatives::CubeWireframe::verticies, Primatives::CubeWireframe::indicies);
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(VertexShader::CreateFromSourceFile(vsfilename, vsLayout));
	model->pMaterial->AddBindable(PixelShader::CreateFromSourceFile(psfilename));
	model->entities.instances.emplace_back(2.0f, 0.0f, 0.0f);

	models.push_back(model);
}

void BallWorld::CreateAxis()
{
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Color", DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT},
	};

	std::vector<Vertex> verticies = {
		{ {0.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {0.0f, 0.0f, 0.0f},  {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {0.0f, 0.0f, 0.0f},  {0.0f, 0.0f, 1.0f, 1.0f} },
		{ {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f, 1.0f} },
	};

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "VSTest.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "PSTest.hlsl";

	auto model = std::make_shared<Model>("Axis");
	model->pMesh = std::make_shared<WireframeList<Vertex>>(verticies);
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	model->pMaterial->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));
	model->entities.instances.emplace_back(0.0f, 0.0f, 0.0f);

	models.push_back(model);
}
