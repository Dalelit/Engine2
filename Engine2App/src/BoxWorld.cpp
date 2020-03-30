#include "pch.h"
#include "BoxWorld.h"
#include "MeshPrimatives.h"
#include "Surface.h"

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
	Engine::Get().mainCamera.SetPosition(7.0f, 9.0f, -10.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	auto model = std::make_shared<Model>("Cube");
	models.push_back(model);

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT},
	};

	Util::Random rng(0.0f,255.0f);

	model->pMesh = std::make_shared<MeshTriangleIndexList<XMFLOAT3>>(MeshPrimatives::Cube::vertexPositions, MeshPrimatives::Cube::indicies);

	Surface2D<XMFLOAT4> tex(2, 2);
	tex.SetValue(0, 0, { 1.0f, 1.0f, 1.0f, 1.0f});
	tex.SetValue(1, 1, { 1.0f, 1.0f, 1.0f, 1.0f });

	model->pMaterial = std::make_shared<Material>("Cube1");

	model->pMaterial->pTexture = std::make_shared<Texture>(0, tex, DXGI_FORMAT_R32G32B32A32_FLOAT);
	model->pMaterial->pTexture->SetSampler(std::make_shared<TextureSampler>(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP));

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BoxWorldCubeVS.hlsl";
	model->pMaterial->pVS = std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout);

	std::string psfilename = Config::directories["ShaderSourceDir"] + "BoxWorldCubePS.hlsl";
	model->pMaterial->pPS = std::make_shared<PixelShaderDynamic>(psfilename);

	pVoxel = std::make_unique<Voxel>(10, 10, 10);

	model->entities.instances.reserve(pVoxel->Size());
	for (auto pPoint : *pVoxel)
	{
		model->entities.instances.emplace_back(pPoint.x, pPoint.y, pPoint.z);
	}
}
