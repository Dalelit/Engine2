#include "pch.h"
#include "BallWorld.h"
#include "Primatives.h"
#include "VertexBufferInstanced.h"

using namespace Engine2;
using namespace DirectX;

BallWorld::BallWorld() : Layer("BallWorld")
{
	Engine::Get().mainCamera.SetPosition(2.0f, 3.0f, -4.0f);
	Engine::Get().mainCamera.LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 0.1f, 0.1f, 0.1f, 1.0f };
	scene.pointLights.emplace_back(PointLight({ -1.0f, 4.0f, -2.0f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }));

	//CreateCube();
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

	VertexLayoutSimple::VertexShaderLayout vsLayout = {
		{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Normal", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
		{"Color", DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT},
	};
	auto layout = VertexLayoutSimple::ToDescriptor(vsLayout);

	std::vector<Vertex> verticies(Primatives::Cube::verticies.size());
	Primatives::CopyPositionNormalColor(verticies, Primatives::Cube::verticies);

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld1VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<Model>("Cube");
	model->pMesh = std::make_shared<MeshTriangleIndexList<Vertex>>(verticies, Primatives::Cube::indicies);
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, layout));
	model->pMaterial->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));

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

	struct InstanceInfo {
		XMFLOAT3 position;
	};
	std::vector<InstanceInfo> instances;

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"InstancePos", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	auto sphereData = Primatives::IcoSphere::CreateIcoSphere(2);
	Primatives::CopyPositionNormal<Vertex>(verticies, *sphereData.verticies);

	instances.resize(100000);
	for (int i = 0; i < instances.size(); i++)
	{
		XMVECTOR p4 = Util::RandomInUnitSphere() * 50.0f;
		XMStoreFloat3(&instances[i].position, p4);
	}

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "BallWorld2VS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "BallWorld1PS.hlsl";

	auto model = std::make_shared<Model>("IcoSphere");
	auto vertBuffer = std::make_shared<TriangleListIndexInstanced<Vertex, InstanceInfo>>(verticies, *sphereData.indicies, instances);
	model->pMesh = vertBuffer;
	model->pMaterial = std::make_shared<RenderNode>("RN1");
	model->pMaterial->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	model->pMaterial->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));

	model->entities.instances.emplace_back(0.0f, 0.0f, 0.0f);

	models.push_back(model);
}
