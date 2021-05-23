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
#include "TextureLoader.h"
#include "UtilFileDialog.h"
#include "ScriptComponent.h"
#include "ScriptComponents/InputControllerScript.h"
#include "Offscreen.h"
#include "AssetManager.h"
#include "ShaderCache.h"

using namespace Engine2;
using namespace EngineECS;
using namespace DirectX;

SceneBuilder::SceneBuilder() : Layer("SceneBuilder")
{
	CreateDefaultAssets();
	CreateEmtpyScene();

	//BuildTestScene();

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

	//{
	//	auto e = scene.CreateEntity("Offscreen test");
	//	auto cc = e.AddComponent<Camera>("Offscreen camera test");
	//	cc->SetAspectRatio(DXDevice::Get().GetAspectRatio());
	//	e.AddComponent<Gizmo>(Gizmo::Types::Camera);
	//	auto buffer = e.AddComponent<Offscreen>();
	//	auto tr = e.GetComponent<Transform>();
	//	tr->position = { 5.0f, 6.3f, 5.0f, 1.0f };
	//	tr->LookAt(0.0f, 1.3f, 0.0f);
	//}
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
	{
		if (Util::FileSelectionDialogue::SaveDialogue(selectedFile))
			SceneSerialisation(scene).SaveScene(selectedFile);
	}
	if (ImGui::Button("Save"))
	{
		SceneSerialisation(scene).SaveScene(selectedFile);
	}
	if (ImGui::Button("Load file..."))
	{
		if (Util::FileSelectionDialogue::LoadDialogue(selectedFile))
		{
			ClearScene();
			SceneSerialisation(scene).LoadScene(selectedFile);
		}
	}
	if (ImGui::Button("Reload"))
	{
		ClearScene();
		SceneSerialisation(scene).LoadScene(selectedFile);
	}

	if (ImGui::Button("Clear Scene"))
	{
		ClearScene();
		CreateEmtpyScene();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Also clear assets", &onClearIncludeAssets);

	scene.OnImgui();
}

void SceneBuilder::CreateDefaultAssets()
{
	auto& asset = AssetManager::Manager().CreateAsset("Default");
	
	asset.Materials().CreateAsset<Materials::StandardMaterial>("Default Material");
}

void SceneBuilder::CreateEmtpyScene()
{
	auto mainCamera = scene.CreateSceneCamera("Main", true);
	auto t = mainCamera.GetComponent<Transform>();
	t->SetPosition(5.0f, 5.0f, -5.0f);
	t->LookAt(0.0f, 0.0f, 0.0f);
	mainCamera.GetComponent<Camera>()->SetFarPlane(50.0f);

	ScriptComponent::RegisterScript<InputControllerScript>("Input Controller");
	auto sc = mainCamera.AddComponent<ScriptComponent>();
	sc->SetEntity(mainCamera); // to do: shouldn't need to do this.
	sc->CreateInstance("Input Controller");
}

void SceneBuilder::BuildTestScene()
{
	 // add a light
	{
		auto e = scene.CreateEntity("Point light 1");
		e.AddComponent<Gizmo>(Gizmo::Types::Sphere);
		e.AddComponent<PointLight>();
		auto tr = e.GetComponent<Transform>();
		tr->SetPosition(-3.0f, 3.0f, -3.0f);
		tr->SetScale(0.25f); // just to make the gizmo smaller
	}

	//{
	//	// textured test
	//	auto e = scene.CreateEntity("Texture plane");
	//	auto mr = e.AddComponent<MeshRenderer>();
	//	mr->mesh = Mesh::Assets["Plane_t"];
	//	e.GetComponent<Transform>()->scale = { 4.0f, 4.0f, 4.0f, 1.0f };

	//	mr->material = Material::Materials.CreateAsset("Textured");
	//	mr->material->vertexShaderCB = std::make_shared<MaterialLibrary::StandardMaterialVSCB>(1);;
	//	mr->material->vertexShader = Material::GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalTextureVS.hlsl", VertexLayout::PositionNormalTexture::Layout);
	//	mr->material->pixelShaderCB = std::make_shared<MaterialLibrary::StandardMaterialPSCB>(1);
	//	mr->material->pixelShader = Material::GetPixelShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalTexturePS.hlsl");
	//	mr->material->texture = TextureLoader::LoadTexture("Assets\\Textures\\NZ Small.jpg");
	//	mr->material->texture->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);
	//	mr->material->texture->SetSlot(2u);
	//}
}

void SceneBuilder::ClearScene()
{
	scene.Clear();

	// clear all the assets.
	// To do: should this be managed in an 'asset manager' class?

	if (onClearIncludeAssets)
	{
		AssetManager::Manager().Clear();
		ShaderCache::VertexShaders.Clear();
		ShaderCache::PixelShaders.Clear();
		TextureLoader::Textures.Clear();
	}
}
