#include "pch.h"
#include "Playground.h"

using namespace Engine2;

void Playground::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void Playground::OnRender()
{
	scene.OnRender();

	for (auto model : models)
	{
		if (model->IsActive())
		{
			model->Bind();
			model->pMesh->Draw();
		}
	}
}

void Playground::OnImgui()
{
	scene.OnImgui();

	ImGui::Checkbox("Active", &active);

	for (auto& model : models) model->OnImgui();
}

void Playground::CreateScene()
{
	scene.mainCamera.SetPosition(0.0f, 0.0f, -3.0f);
	AddModel1(); //models[0]->SetActive(false);
	AddModel2();
	AddModel3();
	AddModel4();
}

void Playground::AddModel1()
{
	auto model = std::make_shared<Model>("Model 1");
	models.push_back(model);

	struct Vertex {
		float x, y, z;
	};

	std::vector<Vertex> verticies = {
		{-0.5f, -0.5f, 0.01f},
		{ 0.0f,  0.5f, 0.01f},
		{ 0.5f, -0.5f, 0.01f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT}
	};

	model->pMesh = std::make_shared<MeshTriangleList<Vertex>>(verticies);

	model->pMaterial = std::make_shared<Material>("Material 1");

	std::string vsCode = R"(
		cbuffer sceneConst
		{
			matrix cameraTransform;
		};

		float4 main(float3 pos : Position) : SV_POSITION
		{
			return mul(float4(pos, 1.0f), cameraTransform);
		})";

	model->pMaterial->pVS = VertexShader::CreateFromString(vsCode, vsLayout);

	std::string psCode = R"(
		float4 main() : SV_TARGET
		{
			return float4(0.8f, 0.2f, 0.2f, 1.0f);
		})";

	model->pMaterial->pPS = PixelShader::CreateFromString(psCode);
}

void Playground::AddModel2()
{
	auto model = std::make_shared<Model>("Model 2");
	models.push_back(model);

	struct Vertex {
		float x, y, z;
		float r, g, b, a;
	};

	std::vector<Vertex> verticies = {
		{-0.9f, -0.5f, -0.01f, 1.0f, 0.0f, 0.0f, 1.0f},
		{-0.5f,  0.5f, -0.01f, 0.0f, 1.0f, 0.0f, 1.0f},
		{ 0.1f, -0.5f, -0.01f, 0.0f, 0.0f, 1.0f, 1.0f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT},
		{"Color", DXGI_FORMAT_R32G32B32A32_FLOAT},
	};

	model->pMesh = std::make_shared<MeshTriangleList<Vertex>>(verticies);

	model->pMaterial = std::make_shared<Material>("Material 2");

	std::string vsCode = scene.GetVSCBHLSL() + R"(
		struct VSOut
		{
			float4 col : Color;
			float4 pos : SV_POSITION;
		};

		VSOut main(float3 pos : Position, float4 col : Color)
		{
			VSOut vso;

			vso.pos = mul(float4(pos, 1.0f), cameraTransform);
			vso.col = col;

			return vso;
		})";

	model->pMaterial->pVS = VertexShader::CreateFromString(vsCode, vsLayout);

	std::string psCode = R"(
		float4 main(float4 col : Color) : SV_TARGET
		{
			return col;
		})";

	model->pMaterial->pPS = PixelShader::CreateFromString(psCode);
}

void Playground::AddModel3()
{
	auto model = std::make_shared<Model>("Model 3");
	models.push_back(model);

	struct Vertex {
		float x, y, z;
	};

	std::vector<Vertex> verticies = {
		{0.0f, -0.50f, 0.02f},
		{0.5f,  0.75f, 0.02f},
		{1.0f, -0.50f, 0.02f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT}
	};

	model->pMesh = std::make_shared<MeshTriangleList<Vertex>>(verticies);

	model->pMaterial = std::make_shared<Material>("Material 3");

	struct ConstData
	{
		float color[4];
	};

	auto pscb = std::make_shared<PSConstantBuffer<ConstData>>();
	pscb->data = { {0.2f, 0.8f, 0.6f, 1.0f} };
	model->pMaterial->resources.push_back(pscb);

	std::string vsCode = scene.GetVSCBHLSL() + R"(
		float4 main(float3 pos : Position) : SV_POSITION
		{
			return mul(float4(pos, 1.0f), cameraTransform);
		})";

	model->pMaterial->pVS = VertexShader::CreateFromString(vsCode, vsLayout);

	std::string psCode = R"(
		cbuffer sceneBuffer
		{
			float4 color;
		}

		float4 main() : SV_TARGET
		{
			return color;
		})";

	model->pMaterial->pPS = PixelShader::CreateFromString(psCode);

	auto* ptr = &pscb->data;
	pscb->ImguiFunc = [ptr]() {
		ImGui::ColorEdit4("Color", ptr->color);
	};
}

void Playground::AddModel4()
{
	auto model = std::make_shared<Model>("Model 4");
	models.push_back(model);

	struct Vertex {
		float x, y, z;
	};

	std::vector<Vertex> verticies = {
		{-0.25f, -0.25f, 0.0f},
		{-0.25f, 0.25f, 0.0f},
		{0.25f, 0.25f, 0.0f},
		{0.25f, -0.25f, 0.0f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT}
	};

	std::vector<unsigned int> indicies = {
		0,1,2,
		0,2,3
	};

	model->pMesh = std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, indicies);

	model->pMaterial = std::make_shared<Material>("Material 4");

	std::string vsCode = scene.GetVSCBHLSL() + R"(

		float4 main(float3 pos : Position) : SV_POSITION
		{
			return mul(float4(pos, 1.0f), cameraTransform);
		})";

	model->pMaterial->pVS = VertexShader::CreateFromString(vsCode, vsLayout);

	std::string psCode = R"(
		float4 main() : SV_TARGET
		{
			return float4(0.7f, 0.7f, 0.2f, 1.0f);
		})";

	model->pMaterial->pPS = PixelShader::CreateFromString(psCode);
}
