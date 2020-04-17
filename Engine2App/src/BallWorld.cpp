#include "pch.h"
#include "BallWorld.h"
#include "Primatives.h"
#include "VertexBufferInstanced.h"

using namespace Engine2;
using namespace DirectX;

BallWorld::BallWorld() : Layer("BallWorld")
{
	Engine::Get().mainCamera.SetPosition(20.0f, 30.0f, -40.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 0.1f, 0.1f, 0.1f, 1.0f };
	scene.pointLights.emplace_back(PointLight({ -1.0f, 4.0f, -2.0f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }));

	CreateCube();
	CreateSphere();
}

void BallWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);

	auto p = positions.begin();
	auto v = velocities.begin();

	for (int i = 0; i < positions.size(); i++)
	{
		auto dv = -(*p);
		*v += dv * 0.25f * dt;
		*p += *v * 0.5f * dt;
		p++;
		v++;
	}

	Engine::UpdateBuffer(positionsBufferPtr, positions);
}

void BallWorld::OnRender()
{
	gizmos.NewFrame();

	scene.OnRender();

	gizmos.DrawAxis({ 0.0f, 0.0f, 0.0f, 1.0f });

	gizmos.DrawSphere(scene.pointLights[0].GetPosition());

	for (auto& m : models) if (m->IsActive()) m->Draw();

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

	struct InstanceInfo {
		XMMATRIX transform;
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal",      0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color",       0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"InstanceTransform", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceTransform", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceTransform", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceTransform", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<Vertex> verticies(Primatives::Cube::verticies.size());
	Primatives::CopyPositionNormalColor(verticies, Primatives::Cube::verticies);

	Util::Random rngPos(-40.0f, 40.0f);
	Util::Random rngRot(-XM_PI, XM_PI);
	Util::Random rngScl(0.5f, 1.5f);
	std::vector<InstanceInfo> instances(100000);
	for (int i = 0; i < instances.size(); i++)
	{
		XMMATRIX transform;
		transform = XMMatrixScaling(rngScl.Next(), rngScl.Next(), rngScl.Next());
		transform *= XMMatrixRotationRollPitchYaw(rngRot.Next(), rngRot.Next(), rngRot.Next());
		transform *= XMMatrixTranslation(rngPos.Next(), rngPos.Next(), rngPos.Next());
		instances[i].transform = XMMatrixTranspose(transform);
	}

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld1VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<ModelEntities>("Cube");
	auto rn = model->AddRenderNode("RN1");
	rn->SetDrawable(std::make_shared<TriangleListIndexInstanced<Vertex, InstanceInfo>>(verticies, Primatives::Cube::indicies, instances));
	rn->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	rn->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));

	model->entities.instances.emplace_back(1.0f, 0.0f, 0.0f);

	models.push_back(model);
}

void BallWorld::CreateSphere()
{
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
	};
	std::vector<Vertex> verticies;

	std::vector<XMFLOAT3> colors;

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal",      0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"InstanceCol", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstancePos", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	auto sphereData = Primatives::IcoSphere::CreateIcoSphere(2);
	Primatives::CopyPositionNormal<Vertex>(verticies, *sphereData.verticies);

	//Util::Random rng(-40.0f, 40.0f);
	positions.resize(10000);
	velocities.resize(positions.size());
	colors.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		positions[i] = Util::RandomInUnitSphere() * 50.f;
		colors[i] = { Util::rng.Next(), Util::rng.Next(), Util::rng.Next() };
		velocities[i] = Util::RandomInUnitSphere();
		velocities[i] *= (g_XMOne - XMVector3Length(velocities[i])) * 200.0f;
	}

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld2VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<Model>("IcoSphere");
	auto rn = model->AddRenderNode("RN sphere");
	auto vertBuffer = std::make_shared<TriangleListIndexInstanced<Vertex, XMFLOAT3>>(verticies, *sphereData.indicies, colors);
	positionsBufferPtr = vertBuffer->AddBuffer<XMVECTOR>(positions, true);
	rn->SetDrawable(vertBuffer);
	rn->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	rn->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));

	models.push_back(model);
}
