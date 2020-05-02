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
	CreateStencil();

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
	stencil.Clear();
	stencil.SetAsTarget();

	scene.OnRender();

	for (auto& m : models) if (m->IsActive()) m->Draw();

	stencil.DrawToBackBuffer();

	//Outline();
}

void ModelTest::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
	if (event.GetType() == Engine2::EventType::WindowResize)
	{
		stencil.Reconfigure();
	}
}

void ModelTest::OnImgui()
{
	ImGui::DragFloat("Outline scale", &outlineScale, 0.05f, 1.0f, 2.0f);
	ImGui::ColorEdit4("Outline color", outlineColor.m128_f32);
	scene.OnImgui();
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

	//stencil.SetAsTarget();
	//float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
	//DXDevice::GetContext().ClearRenderTargetView(stencil.GetRenderTarget().pTargetView.Get(), clearColor);
	//DXDevice::GetContext().ClearDepthStencilView(pDSVStencil.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0u);

	//pVSOutline->Bind(); // used for both

	//vsOutlineCB.data.m128_f32[0] = 1.0f;
	//vsOutlineCB.Bind();
	//DXDevice::GetContext().PSSetShader(nullptr, nullptr, 0);
	//DXDevice::GetContext().OMSetRenderTargets(0u, nullptr, pDSVStencil.Get());
	//DXDevice::GetContext().OMSetDepthStencilState(pDSSWrite.Get(), 0xFF);

	//pSelectedModel->RenderDrawablesOnly();

	//vsOutlineCB.data.m128_f32[0] = outlineScale;
	//vsOutlineCB.Bind();
	//psOutlineCB.data = outlineColor;
	//psOutlineCB.Bind();
	//pPSOutline->Bind();
	//DXDevice::GetContext().OMSetRenderTargets(1u, stencil.GetRenderTarget().pTargetView.GetAddressOf(), pDSVStencil.Get());
	//DXDevice::GetContext().OMSetDepthStencilState(pDSSMask.Get(), 0xFF);

	//pSelectedModel->RenderDrawablesOnly();

	//DXDevice::GetContext().OMSetBlendState(pBlendState.Get(), nullptr, 0xffffffff);
	//stencil.Draw();
	//DXDevice::GetContext().OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void ModelTest::CreateStencil()
{
	HRESULT hr;

	vsOutlineCB.slot = 1;

	D3D11_DEPTH_STENCIL_DESC stencilWriteDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	D3D11_DEPTH_STENCIL_DESC stencilMaskDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	D3D11_DEPTH_STENCILOP_DESC readOps;
	readOps.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	readOps.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	// note: read/write masks set to 0xFF
	
	// Write
	readOps.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;
	readOps.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	stencilWriteDesc.DepthEnable = false;
	stencilWriteDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	stencilWriteDesc.StencilEnable = true;
	stencilWriteDesc.StencilWriteMask = 0xFF;
	stencilWriteDesc.FrontFace = readOps;
	stencilWriteDesc.BackFace = readOps;
	hr = DXDevice::GetDevice().CreateDepthStencilState(&stencilWriteDesc, &pDSSWrite);
	E2_ASSERT_HR(hr, "CreateDepthStencilState for stencil write failed");

	// Mask
	readOps.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	readOps.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
	stencilMaskDesc.DepthEnable = false;
	stencilMaskDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	stencilMaskDesc.StencilEnable = true;
	stencilMaskDesc.StencilReadMask = 0xFF;
	stencilMaskDesc.FrontFace = readOps;
	stencilMaskDesc.BackFace = readOps;

	hr = DXDevice::GetDevice().CreateDepthStencilState(&stencilMaskDesc, &pDSSMask);
	E2_ASSERT_HR(hr, "CreateDepthStencilState for stencil Mask failed");

	// Depth stencil

	D3D11_TEXTURE2D_DESC dtDesc = {};
	dtDesc.Width =  DXDevice::Get().GetBackBufferTextureDesc().Width;
	dtDesc.Height = DXDevice::Get().GetBackBufferTextureDesc().Height;
	dtDesc.MipLevels = 1u;
	dtDesc.ArraySize = 1u;
	dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	dtDesc.SampleDesc.Count = 1u;
	dtDesc.SampleDesc.Quality = 0u;
	dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	dtDesc.CPUAccessFlags = 0u;
	dtDesc.MiscFlags = 0u;

	hr = DXDevice::GetDevice().CreateTexture2D(&dtDesc, nullptr, &pDTStencil);

	E2_ASSERT_HR(hr, "CreateTexture2D failed");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = dtDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2D.MipSlice = 0u;

	hr = DXDevice::GetDevice().CreateDepthStencilView(pDTStencil.Get(), &dsvDesc, &pDSVStencil);

	E2_ASSERT_HR(hr, "CreateDepthStencilView failed");

	// shaders for outline
	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	std::string vsFileName = Config::directories["ShaderCompiledDir"] + "OutlineVS.cso";
	pVSOutline = VertexShaderDynamic::CreateFromCompiledFile(vsFileName, vsLayout);

	std::string psFileName = Config::directories["ShaderCompiledDir"] + "OutlinePS.cso";
	pPSOutline = PixelShaderDynamic::CreateFromCompiledFile(psFileName);

	// blend state
	D3D11_BLEND_DESC bsDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	auto& bsrt = bsDesc.RenderTarget[0];
	bsrt.BlendEnable = true;
	bsrt.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	bsrt.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	hr = DXDevice::GetDevice().CreateBlendState(&bsDesc, &pBlendState);

	E2_ASSERT_HR(hr, "CreateBlendState failed");
}
