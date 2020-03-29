#include "pch.h"
#include "BoxWorld.h"
#include "MeshPrimatives.h"

using namespace Engine2;
using namespace DirectX;

void BoxWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void BoxWorld::OnRender()
{
	scene.OnRender();

	for (auto& model : models)
	{
		if (model->IsActive()) model->OnRender();
	}
}

void BoxWorld::OnImgui()
{
	scene.OnImgui();

	ImGui::Checkbox("Active", &active);

	for (auto& model : models) model->OnImgui();
}

void BoxWorld::CreateScene()
{
	Engine::Get().mainCamera.SetPosition(0.0f, 0.0f, -3.0f);

	auto model = std::make_shared<Model>("Cube");
	models.push_back(model);

	struct Vertex
	{
		XMFLOAT3 position;
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT}
	};

	std::vector<Vertex> verticies;
	verticies.reserve(MeshPrimatives::Cube::vertexPositions.size());
	for (auto p : MeshPrimatives::Cube::vertexPositions) verticies.emplace_back(Vertex({ p }));

	model->pMesh = std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, MeshPrimatives::Cube::indicies);

	model->pMaterial = std::make_shared<Material>("Cube1");

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BoxWorldCubeVS.hlsl";
	model->pMaterial->pVS = std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout);

	std::string psfilename = Config::directories["ShaderSourceDir"] + "BoxWorldCubePS.hlsl";
	model->pMaterial->pPS = std::make_shared<PixelShaderDynamic>(psfilename);

	model->entities.instances.reserve(3);
	model->entities.instances.emplace_back(Entity());
}
