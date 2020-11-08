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
	static std::string selectedFile = "Scenes//testScene.txt";
	ImGui::Text("File: %s", selectedFile.c_str());
	if (ImGui::Button("Save as..."))
		if (FileSelectionDialogue::SaveDialogue(selectedFile))
			SceneSerialisation(scene).SaveScene(selectedFile);
	if (ImGui::Button("Save"))
		SceneSerialisation(scene).SaveScene(selectedFile);
	if (ImGui::Button("Load file..."))
		if (FileSelectionDialogue::LoadDialogue(selectedFile))
			SceneSerialisation(scene).LoadScene(selectedFile);
	if (ImGui::Button("Reload"))
		SceneSerialisation(scene).LoadScene(selectedFile);

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
		e.GetComponent<Transform>()->position = { -3.0f, 3.0f, -3.0f, 0.0f };
	}

	{
		auto e = scene.CreateEntity();
		auto mr = e.AddComponent<MeshRenderer>();
		mr->mesh = Mesh::Assets["Sphere"];
		mr->material = Material::Materials["Default PNC"];
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

	Material::Materials.CreateAsset<MaterialLibrary::PositionNormalColorMaterial>("Default PNC");
	Material::Materials.CreateAsset<MaterialLibrary::PositionNormalColorWireframe>("Wireframe PNC");
}
