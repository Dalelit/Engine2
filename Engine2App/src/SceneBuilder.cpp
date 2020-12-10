#include "pch.h"
#include "SceneBuilder.h"
#include "Engine2.h"
#include "Components.h"
#include "Transform.h"
#include "Particles.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Lights.h"
#include "VertexLayout.h"
#include "SceneSerialisation.h"
#include "MaterialLibrary.h"
#include "TextureLoader.h"
#include "UtilFileDialog.h"
#include "ScriptComponent.h"
#include "ScriptComponents/InputControllerScript.h"
#include "Offscreen.h"

using namespace Engine2;
using namespace EngineECS;
using namespace DirectX;

SceneBuilder::SceneBuilder() : Layer("SceneBuilder")
{
	{
		ScriptComponent::RegisterScript<InputControllerScript>("Input Controller");
	}

	{
		auto mainCamera = scene.CreateSceneCamera("Main", true);
		auto t = mainCamera.GetComponent<Transform>();
		t->SetPosition(5.0f, 5.0f, -5.0f);
		t->LookAt(0.0f, 0.0f, 0.0f);

		auto sc = mainCamera.AddComponent<ScriptComponent>();
		sc->SetEntity(mainCamera);
		sc->CreateInstance("Input Controller");
	}

	LoadPrimatives();
	BuildTestScene();

	scene.GetSkybox().Initialise("Assets\\Skyboxes\\Test");
	//std::vector<std::string> files = {
	//	"Assets\\Skyboxes\\Corona\\corona_ft.png",
	//	"Assets\\Skyboxes\\Corona\\corona_bk.png",
	//	"Assets\\Skyboxes\\Corona\\corona_up.png",
	//	"Assets\\Skyboxes\\Corona\\corona_dn.png",
	//	"Assets\\Skyboxes\\Corona\\corona_rt.png",
	//	"Assets\\Skyboxes\\Corona\\corona_lf.png",
	//};
	//scene.GetSkybox().Initialise(files);

	{
		auto e = scene.CreateEntity("Offscreen test");
		auto cc = e.AddComponent<Camera>("Offscreen camera test");
		cc->SetAspectRatio(DXDevice::Get().GetAspectRatio());
		e.AddComponent<Gizmo>(Gizmo::Types::Camera);
		auto buffer = e.AddComponent<Offscreen>();
		auto tr = e.GetComponent<Transform>();
		tr->position = { 5.0f, 6.3f, 5.0f, 1.0f };
		tr->LookAt(0.0f, 1.3f, 0.0f);
	}

	{
		auto e = scene.CreateEntity("Directional Light Test");
		auto dl = e.AddComponent<DirectionalLight>();
	}
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
		if (Util::FileSelectionDialogue::SaveDialogue(selectedFile))
			SceneSerialisation(scene).SaveScene(selectedFile);
	if (ImGui::Button("Save"))
		SceneSerialisation(scene).SaveScene(selectedFile);
	if (ImGui::Button("Load file..."))
		if (Util::FileSelectionDialogue::LoadDialogue(selectedFile))
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
		auto e = scene.CreateEntity("Point light 1");
		e.AddComponent<Gizmo>(Gizmo::Types::Sphere);
		e.AddComponent<PointLight>();
		auto tr = e.GetComponent<Transform>();
		tr->SetPosition(-3.0f, 3.0f, -3.0f);
		tr->SetScale(0.25f); // just to make the gizmo smaller
	}

	{
		auto e = scene.CreateEntity("Sphere");
		auto mr = e.AddComponent<MeshRenderer>();
		mr->mesh = Mesh::Assets["Sphere"];
		mr->material = Material::Materials["Default PNC"];
		e.GetComponent<Transform>()->position = { 0.0f, 1.3f, 0.0f, 1.0f };
	}

	{
		auto e = scene.CreateEntity("Cube");
		auto mr = e.AddComponent<MeshRenderer>();
		mr->mesh = Mesh::Assets["Cube"];
		mr->material = Material::Materials["Default PNC"]->Clone();
		auto cb = std::static_pointer_cast<PSConstantBuffer<MaterialLibrary::StandardMaterial>>(mr->material->pixelShaderCB);
		if (cb) cb->data.diffuse = { 0.8f, 0.2f, 0.2f };
		e.GetComponent<Transform>()->position = { -2.0f, 2.0f, -1.0f, 1.0f };
	}

	{
		auto e = scene.CreateEntity("Plane plane");
		auto mr = e.AddComponent<MeshRenderer>();
		mr->mesh = Mesh::Assets["Plane"];
		mr->material = Material::Materials["Default PNC"]->Clone();
		auto cb = std::static_pointer_cast<PSConstantBuffer<MaterialLibrary::StandardMaterial>>(mr->material->pixelShaderCB);
		if (cb) cb->data.diffuse = { 0.2f, 0.8f, 0.2f };
		auto tr = e.GetComponent<Transform>();
		tr->SetPosition(0.0f, -2.0f, 0.0f);
		tr->SetScale(20.0f);
	}

	{
		// textured test
		auto e = scene.CreateEntity("Texture plane");
		auto mr = e.AddComponent<MeshRenderer>();
		mr->mesh = Mesh::Assets["Plane_t"];
		e.GetComponent<Transform>()->scale = { 4.0f, 4.0f, 4.0f, 1.0f };

		mr->material = Material::Materials.CreateAsset("Textured");
		mr->material->vertexShaderCB = std::make_shared<MaterialLibrary::StandardMaterialVSCB>(1);;
		mr->material->vertexShader = Material::GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalTextureVS.hlsl", VertexLayout::PositionNormalTexture::Layout);
		mr->material->pixelShaderCB = std::make_shared<MaterialLibrary::StandardMaterialPSCB>(1);
		mr->material->pixelShader = Material::GetPixelShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalTexturePS.hlsl");
		mr->material->texture = TextureLoader::LoadTexture("Assets\\Textures\\NZ Small.jpg");
		mr->material->texture->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);
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
