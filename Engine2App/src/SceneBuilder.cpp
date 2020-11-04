#include "pch.h"
#include "SceneBuilder.h"
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
#include "SceneSerialisation.h"
#include "MaterialLibrary.h"

using namespace Engine2;
using namespace EngineECS;
using namespace DirectX;

SceneBuilder::SceneBuilder() : Layer("SceneBuilder")
{
	Engine::GetActiveCamera().SetPosition(5.0f, 5.0f, -5.0f);
	Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 0.2f, 0.2f, 0.2f, 1.0f };

	LoadPrimatives();
	BuildTestScene();
}

void SceneBuilder::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void SceneBuilder::OnRender()
{
	scene.OnRender();
}

void SceneBuilder::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
	scene.OnApplicationEvent(event);
}

void SceneBuilder::OnImgui()
{
	char buffer[256] = "Scenes//testScene.txt";
	ImGui::InputText("Filename", buffer, sizeof(buffer));
	if (ImGui::Button("Save")) SceneSerialisation(scene).SaveScene(buffer);
	if (ImGui::Button("Load")) SceneSerialisation(scene).LoadScene(buffer);

	scene.OnImgui();
}

void SceneBuilder::BuildTestScene()
{
	RigidBody::gravity = g_XMZero;

	// add a light
	{
		auto e = scene.CreateEntity();
		e.AddComponent<Gizmo>()->type = Gizmo::Types::Sphere;
		e.AddComponent<PointLight>();
		e.GetComponent<Transform>()->Set(-3.0f, 3.0f, -3.0f);
	}

	// add a particle emitter
	{
		//auto e = scene.CreateEntity();
		//auto pe = e.AddComponent<ParticleEmitter>();
		//pe->SetMaxParticles(2000);
		//pe->SetRate(500.0f);
		//e.GetComponent<Transform>()->Set(-2.0f, 0.0f, 2.0f);
	}

	using Vertex = VertexLayout::PositionNormalColor::Vertex;
	auto vsLayout = VertexLayout::PositionNormalColor::GetLayout();

	return;

	// sphere
	{
		auto ico = Primatives::IcoSphere::CreateIcoSphere(3);
		std::vector<Vertex> verticies(ico.verticies->size());
		Primatives::CopyPositionNormal(verticies, *ico.verticies);
		for (auto& vi : verticies) vi.color = { 0.3f, 0.6f, 0.7f, 1.0f };

		auto m = Mesh::Assets.CreateAsset("Icosphere3");
		m->SetDrawable<MeshTriangleIndexList<Vertex>>(verticies, *ico.indicies);
	}

	// cube
	{
		std::vector<Vertex> verticies(Primatives::Cube::verticies.size());
		Primatives::CopyPositionNormalColor(verticies, Primatives::Cube::verticies);

		auto m = Mesh::Assets.CreateAsset("Cube");
		m->SetDrawable<MeshTriangleIndexList<Vertex>>(verticies, Primatives::Cube::indicies);
	}

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

	// load a model
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
}

void SceneBuilder::LoadPrimatives()
{
	scene.LoadModel("Assets\\Models\\Cube.obj");
	scene.LoadModel("Assets\\Models\\Cone.obj");
	scene.LoadModel("Assets\\Models\\Cylinder.obj");
	scene.LoadModel("Assets\\Models\\Plane.obj");
	scene.LoadModel("Assets\\Models\\Sphere.obj");
	scene.LoadModel("Assets\\Models\\Torus.obj");

	auto mat = Material::Assets.CreateAsset<MaterialLibrary::PositionNormalColorMaterial>("Default PNC");
}
