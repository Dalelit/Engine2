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
	constexpr const char* sceneFilename = "\\Scene.txt";

	ImGui::Text("Scene dir: %s", sceneDirectory.c_str());

	auto LoadSceneHelper = [&]() {
		std::string filename = sceneDirectory + sceneFilename;

		ClearScene(true);

		if (Util::FileExists(filename))
		{
			SceneSerialisation(scene).LoadScene(sceneDirectory + sceneFilename);
		}
	};

	if (ImGui::Button("Scene Create..."))
	{
		if (Util::FileSelectionDialogue::SelectFolderDialogue(sceneDirectory))
		{
			SetWorkingDirectory(sceneDirectory);
			ClearScene();
			CreateEmtpyScene();
		}
	}

	if (ImGui::Button("Scene Open..."))
	{
		if (Util::FileSelectionDialogue::SelectFolderDialogue(sceneDirectory))
		{
			SetWorkingDirectory(sceneDirectory);
			LoadSceneHelper();
		}
	}
	if (!sceneDirectory.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Scene Reload"))
		{
			LoadSceneHelper();
		}
	}

	if (ImGui::Button("Scene Save as..."))
	{
		if (Util::FileSelectionDialogue::SelectFolderDialogue(sceneDirectory))
		{
			SetWorkingDirectory(sceneDirectory); // to do: this doesn't move the assets
			SceneSerialisation(scene).SaveScene(sceneDirectory + sceneFilename);
		}
	}
	if (!sceneDirectory.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Scene Save"))
		{
			SceneSerialisation(scene).SaveScene(sceneDirectory + sceneFilename);
		}
	}

	if (ImGui::Button("Clear Scene"))
	{
		ClearScene();
		sceneDirectory = "";
		SetWorkingDirectory(sceneDirectory);
		CreateEmtpyScene();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Also clear assets", &onClearIncludeAssets);

	ImGui::Separator();
	if (!sceneDirectory.empty())
	{
		ImGuiSceneDirectoy();
	}

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

void SceneBuilder::ClearScene(bool forceClearAsset)
{
	scene.Clear();

	// clear all the assets.
	// To do: should this be managed in an 'asset manager' class?

	if (forceClearAsset || onClearIncludeAssets)
	{
		AssetManager::Manager().Clear();
		ShaderCache::VertexShaders.Clear();
		ShaderCache::PixelShaders.Clear();
		TextureLoader::Textures.Clear();
	}
}

void SceneBuilder::LoadAsset(const std::string& dirname, const std::string& filename)
{
	std::string fullPath = dirname + "\\" + filename;
	//std::string assetName = filename.string().substr(sceneDirectory.size()+1);
	
	AssetManager::Manager().LoadModel(fullPath);
}

void SceneBuilder::ImGuiSceneDirectoy()
{
	// to do: don't check if someone deletes the directory we're in.

	if (ImGui::CollapsingHeader("Scene directory content", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (sceneDirectory != currentDirectory)
		{
			if (ImGui::MenuItem(".."))
			{
				SetWorkingDirectory(std::filesystem::path(currentDirectory).parent_path().string());
			}
		}
		//else
		//{
		//	ImGui::Text(currentDirectory.c_str());
		//}

		FileInfo* pChangeDirectory = nullptr;
		for (auto& f : currentDirectoryContents)
		{
			bool selected = f.name == selectedFile;
			if (ImGui::MenuItem(f.name.c_str(), nullptr, &selected))
			{
				if (f.isDirectory)
				{
					pChangeDirectory = &f;
				}
				else
				{
					selectedFile = f.name;
				}
			}
		}
		
		ImGui::Separator();
		if (!selectedFile.empty())
		{
			ImGui::Text(selectedFile.c_str());
			ImGui::SameLine();
			if (ImGui::Button("Action"))
			{
				if (Util::StringEndsWith(selectedFile, ".obj"))
				{
					LoadAsset(currentDirectory, selectedFile);
				}
			}
		}
		if (ImGui::Button("Refresh directory")) SetWorkingDirectory(currentDirectory);

		if (pChangeDirectory) SetWorkingDirectory(currentDirectory + pChangeDirectory->name);
	}
}

void SceneBuilder::SetWorkingDirectory(const std::string& dir)
{
	currentDirectory = dir;
	currentDirectoryContents.clear();
	selectedFile.clear();

	if (!dir.empty())
	{
		for (auto& f : std::filesystem::directory_iterator(currentDirectory))
		{
			auto& info = currentDirectoryContents.emplace_back();
			info.isDirectory = f.is_directory();

			if (info.isDirectory) info.name = "\\" + f.path().filename().string();
			else info.name = f.path().filename().string();
		}
	}
}
