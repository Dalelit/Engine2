#include "pch.h"
#include "ECSTest.h"
#include "Primatives.h"
#include "Engine2.h"
#include "VertexBuffer.h"
#include "Components.h"
#include "Particles.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Lights.h"
#include "VertexLayout.h"
#include "AssetLoaders/ObjLoader.h"

using namespace Engine2;
using namespace EngineECS;
using namespace DirectX;

ECSTest::ECSTest() : Layer("ECSTest")
{
	Engine::GetActiveCamera().SetPosition(5.0f, 5.0f, -5.0f);
	Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 0.2f, 0.2f, 0.2f, 1.0f };

	auto e = scene.CreateEntity();
	e.AddComponent<Gizmo>()->type = Gizmo::Types::Sphere;
	e.AddComponent<PointLight>();

	CreateCube();
	CreateModel();

	//auto e = scene.CreateEntity();
	//auto pe = e.AddComponent<ParticleEmitter>();
	//pe->SetMaxParticles(2000);
	//pe->SetRate(500.0f);
	//e.GetComponent<Transform>()->Set(-2.0f, 0.0f, 2.0f);
}

void ECSTest::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void ECSTest::OnRender()
{
	scene.OnRender();
}

void ECSTest::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
	scene.OnApplicationEvent(event);
}

void ECSTest::OnImgui()
{
	scene.OnImgui();
}

void ECSTest::CreateCube()
{
	using Vertex = VertexLayout::PositionNormalColor::Vertex;
	auto vsLayout = VertexLayout::PositionNormalColor::GetLayout();

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "StandardPosNorColVS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "StandardPosNorColPS.hlsl";

	{
		auto ico = Primatives::IcoSphere::CreateIcoSphere(3);
		std::vector<Vertex> v(ico.verticies->size());
		Primatives::CopyPositionNormal(v, *ico.verticies);
		for (auto& vi : v) vi.color = {0.3f, 0.6f, 0.7f, 1.0f};

		auto m = Mesh::Assets.CreateAsset("Icosphere3");
		m->SetDrawable<MeshTriangleIndexList<Vertex>>(v, *ico.indicies);
	}

	{
		std::vector<Vertex> verticies(Primatives::Cube::verticies.size());
		Primatives::CopyPositionNormalColor(verticies, Primatives::Cube::verticies);

		auto m = Mesh::Assets.CreateAsset("Cube");
		m->SetDrawable<MeshTriangleIndexList<Vertex>>(verticies, Primatives::Cube::indicies);
	}

	{
		auto mat = Material::Assets.CreateAsset("forPrimatives");
		mat->vertexShaderCB = std::make_shared<VSConstantBuffer<Transform>>(1);
		mat->vertexShader = std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout);
		mat->pixelShader = std::make_shared<PixelShaderDynamic>(psfilename);
	}

	RigidBody::gravity = g_XMZero;

	{
		auto e = scene.CreateEntity();
		e.GetComponent<Transform>()->Set(-3.0f, 0.0f, 0.0f);
		auto mr = e.AddComponent<MeshRenderer>();
		mr->mesh = Mesh::Assets["Cube"];
		mr->material = Material::Assets["forPrimatives"];

		auto rb = e.AddComponent<RigidBody>();
		rb->angularVelocity = { 0.5f , 0.0f, 0.0f, 0.0f };
		e.AddComponent<Gizmo>()->type = Gizmo::Types::Axis;
	}

	{
		auto e2 = scene.CreateEntity();
		auto mr2 = e2.AddComponent<MeshRenderer>();
		mr2->mesh = Mesh::Assets["Icosphere3"];
		mr2->material = Material::Assets["forPrimatives"];
		e2.GetComponent<Transform>()->Set(3.0f, 0.0f, 0.0f, 1.5f, 1.5f, 1.5f, 15.0f, 30.0f, 45.0f);
		e2.AddComponent<RigidBody>()->angularVelocity = { 0.0f , -0.8f, 0.3f, 0.0f };
		e2.AddComponent<Gizmo>()->type = Gizmo::Types::Axis;
	}
}

void ECSTest::CreateModel()
{
	using Vertex = VertexLayout::PositionNormalColor::Vertex;
	auto vsLayout = VertexLayout::PositionNormalColor::GetLayout();

	auto loadedModel = AssetLoaders::ObjLoader::Load(Engine2::Config::directories["ModelsDir"] + "torusSmooth.obj");

	auto assets = MeshAssetLoader::CreateMeshAsset(*loadedModel);

	auto e = scene.CreateEntity();
	auto mr = e.AddComponent<MeshRenderer>();
	mr->mesh = Mesh::Assets[assets[0]]; // to do: hack to the first one.
	mr->material = Material::Assets["forPrimatives"];
}
