#include "pch.h"
#include "Playground.h"

using namespace Engine2;

void Playground::OnUpdate(float dt)
{
}

void Playground::OnRender()
{
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
	ImGui::Checkbox("Active", &active);

	if (ImGui::TreeNode("Models"))
	{
		for (auto& model : models)
		{
			model->OnImgui();
		}

		ImGui::TreePop();
	}
}

void Playground::CreateScene()
{
	AddModel1();
	AddModel2();
	AddModel3();
}

void Playground::AddModel1()
{
	auto model = std::make_shared<Model>("Model 1");
	models.push_back(model);

	struct Vertex {
		float x, y, z;
	};

	std::vector<Vertex> verticies = {
		{-0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT}
	};

	model->pMesh = std::make_shared<MeshTriangleList<Vertex>>(verticies);

	model->pMaterial = std::make_shared<Material>("Material 1");

	std::string vsCode = R"(
		float4 main(float3 pos : Position) : SV_POSITION
		{
			return float4(pos, 1.0f);
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
		{-0.9f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
		{0.1f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT},
		{"Color", DXGI_FORMAT_R32G32B32A32_FLOAT},
	};

	model->pMesh = std::make_shared<MeshTriangleList<Vertex>>(verticies);

	model->pMaterial = std::make_shared<Material>("Material 2");

	std::string vsCode = R"(
		struct VSOut
		{
			float4 col : Color;
			float4 pos : SV_POSITION;
		};

		VSOut main(float3 pos : Position, float4 col : Color)
		{
			VSOut vso;

			vso.pos = float4(pos, 1.0f);
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
		{0.0f, -0.5f, 0.0f},
		{0.5f, 0.75f, 0.0f},
		{1.0f, -0.5f, 0.0f},
	};

	VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT_R32G32B32_FLOAT}
	};

	model->pMesh = std::make_shared<MeshTriangleList<Vertex>>(verticies);

	struct ConstData
	{
		float r, g, b, a;
	};

	model->pMaterial = std::make_shared<Material>("Material 3");

	auto pscb = std::make_shared<PSConstantBuffer<ConstData>>();
	pscb->data = { 0.2f, 8.0f, 0.6f, 1.0f };
	model->pMaterial->resources.push_back(pscb);

	std::string vsCode = R"(
		float4 main(float3 pos : Position) : SV_POSITION
		{
			return float4(pos, 1.0f);
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
}
