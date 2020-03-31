#include "pch.h"
#include "BoxWorld.h"
#include "MeshPrimatives.h"
#include "Surface.h"

using namespace Engine2;
using namespace DirectX;

BoxWorld::BoxWorld() : Layer("BoxWorld")
{
	Engine::Get().mainCamera.SetPosition(2.0f, 3.0f, -4.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	GSTestScene();
	CreateScene();
}

void BoxWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void BoxWorld::OnRender()
{
	scene.OnRender();

	pGSTestModel->entities.instances[0].LoadTransformT(pGSCB->data.entityTransform, pGSCB->data.entityTransformRotation);
	Engine::Get().mainCamera.LoadViewProjectionMatrixT(pGSCB->data.cameraTransform);
	pGSCB->Bind();

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

	auto model = std::make_shared<Model>("Voxel Cube");
	models.push_back(model);

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT},
	};

	Util::Random rng(0.0f,255.0f);

	model->pMesh = std::make_shared<MeshTriangleIndexList<XMFLOAT3>>(MeshPrimatives::Cube::vertexPositions, MeshPrimatives::Cube::indicies);

	Surface2D<XMFLOAT4> tex(2, 2);
	tex.Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	tex.SetValue(0, 0, { 1.0f, 1.0f, 1.0f, 1.0f });
	tex.SetValue(1, 1, { 1.0f, 1.0f, 1.0f, 1.0f });

	model->pMaterial = std::make_shared<Material>("Cube1");

	model->pMaterial->pTexture = std::make_shared<Texture>(0, tex, DXGI_FORMAT_R32G32B32A32_FLOAT);
	model->pMaterial->pTexture->SetSampler(std::make_shared<TextureSampler>(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP));

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BoxWorldCubeVS.hlsl";
	model->pMaterial->pVS = std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout);

	std::string psfilename = Config::directories["ShaderSourceDir"] + "BoxWorldCubePS.hlsl";
	model->pMaterial->pPS = std::make_shared<PixelShaderDynamic>(psfilename);

	pVoxel = std::make_unique<Voxel>(3, 3, 3);

	model->entities.instances.reserve(pVoxel->Size());
	for (auto pPoint : *pVoxel)
	{
		model->entities.instances.emplace_back(pPoint.x, pPoint.y, pPoint.z);
	}
}

void BoxWorld::GSTestScene()
{
	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT},
	};

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BW_GSTest_VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BW_GSTest_PS.hlsl";
	std::string gsfilename = Config::directories["ShaderSourceDir"] + "BW_GSTest_GS.hlsl";

	auto model = std::make_shared<Model>("GS +4 Cube");
	model->pMesh = std::make_shared<MeshTriangleIndexList<XMFLOAT3>>(MeshPrimatives::Cube::vertexPositions, MeshPrimatives::Cube::indicies);
	model->pMaterial = std::make_shared<Material>("VSGSPSTest");
	model->pMaterial->pVS = std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout);
	model->pMaterial->pPS = std::make_shared<PixelShaderDynamic>(psfilename);
	model->pMaterial->pGS = std::make_shared<GeometryShaderDynamic>(gsfilename);
	model->entities.instances.emplace_back(4.0f, 0.0f, 0.0f);
	models.push_back(model);

	pGSTestModel = model; // hack to make it easy to update
	pGSCB = std::make_shared<GSConstantBuffer<GSConstantData>>(1u);
}
