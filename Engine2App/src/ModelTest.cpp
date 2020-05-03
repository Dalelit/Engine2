#include "pch.h"
#include "ModelTest.h"
#include "Shader.h"
#include "Engine2.h"

using namespace Engine2;
using namespace DirectX;

ModelTest::ModelTest() : Engine2::Layer("ModelTest")
{
	//Engine::Get().ImguiActive(false);
	//gizmos.SetActive(false);

	Engine::GetActiveCamera().SetPosition(7.5f, 9.7f, -5.0f);
	Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);

	//Camera* camera = Engine::Get().AddGetCamera("back");
	//camera->SetPosition(-0.5f, 0.7f, 1.0f);
	//camera->LookAt(0.0f, 0.0f, 0.0f);

	scene.psConstBuffer.data.ambientLight = { 0.1f, 0.1f, 0.1f, 1.0f };
	scene.pointLights.emplace_back(PointLight({ -1.0f, 4.0f, -2.0f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }));

	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 color;
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal",      0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color",       0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	//auto loadedModel = AssetLoaders::ObjLoader::Load(Engine2::Config::directories["ModelsDir"] + "PicaPica.obj");   loadedModel->ScaleVerticies(100.0f);
	auto loadedModel = AssetLoaders::ObjLoader::Load(Engine2::Config::directories["ModelsDir"] + "torusSmooth.obj");

	auto model = new Model(loadedModel->filename);
	models.push_back(model);
	pSelectedModel = model;

	std::string vsfilename = Config::directories["ShaderSourceDir"] + "ModelTestVS.hlsl";
	std::string psfilename = Config::directories["ShaderSourceDir"] + "ModelTestPS.hlsl";

	auto node1 = model->AddRenderNode("Shaders");
	node1->AddBindable(std::make_shared<VertexShaderDynamic>(vsfilename, vsLayout));
	node1->AddBindable(std::make_shared<PixelShaderDynamic>(psfilename));

	for (auto& kv : loadedModel->objects)
	{
		auto& obj = loadedModel->objects[kv.first];
		if (obj.facesV.size() == obj.facesVn.size())
		{
			std::vector<Vertex> verticies;
			verticies.reserve(200000);

			Vertex v;
			v.color = loadedModel->materials[obj.material].Kd;;

			size_t num = obj.facesV.size();
			for (size_t i = 0; i < num; i++)
			{
				v.position = loadedModel->verticies[obj.facesV[i]];
				v.normal = loadedModel->normals[obj.facesVn[i]];
				verticies.push_back(v);
			}

			auto node = model->AddRenderNode(obj.name);
			node->SetDrawable(std::make_shared<MeshTriangleList<Vertex>>(verticies));
		}
	}
}

ModelTest::~ModelTest()
{
	for (Model* pm : models) delete pm;
}

void ModelTest::OnUpdate(float dt)
{
	scene.OnUpdate(dt);
}

void ModelTest::OnRender()
{
	outliner.Clear();

	scene.OnRender();

	for (auto& m : models) if (m->IsActive()) m->Draw();

	Outline();
}

void ModelTest::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
	if (event.GetType() == Engine2::EventType::WindowResize)
	{
		outliner.Reconfigure();
	}
}

void ModelTest::OnImgui()
{
	scene.OnImgui();
	outliner.OnImgui();
	for (auto& m : models) m->OnImgui();
}

void ModelTest::OnGizmos()
{
	gizmos.DrawAxis(XMMatrixIdentity());
	for (auto& l : scene.pointLights) gizmos.DrawSphere(l.GetTransform());

	auto currentCam = Engine::Get().CurrentCamera();
	for (auto& pCam : Engine::Get().GetCameras())
	{
		if (currentCam != pCam.get())
		{
			gizmos.DrawCamera(pCam->GetTransform());
		}
	}
}

void ModelTest::Outline()
{
	if (!pSelectedModel) return;

	outliner.SetForWriteMask();

	pSelectedModel->RenderDrawablesOnly();

	outliner.SetForOutline();

	pSelectedModel->RenderDrawablesOnly();

	outliner.DrawToBackBuffer();
}

