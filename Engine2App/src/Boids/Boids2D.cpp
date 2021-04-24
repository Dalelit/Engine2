#include "pch.h"
#include "Boids2D.h"
#include "Surface.h"
#include "VertexLayout.h"

using namespace Engine2;
using namespace DirectX;

Boids2D::Boids2D() : Layer("Boids2D")
{
	controlBuffer.data.time = 0.0f;
	controlBuffer.data.deltaTime = 0.0f;

	InitialiseGfx();
	InitialiseCS();
}

void Boids2D::OnUpdate(float dt)
{
	input.OnUpdate(dt);

	controlBuffer.data.time += dt;
	controlBuffer.data.deltaTime = dt;
	controlBuffer.data.xMouse = input.State.MouseClientPosition.x;
	controlBuffer.data.yMouse = input.State.MouseClientPosition.y;
	controlBuffer.Bind(); // does the update buffer before binding

	boiduav.Bind();
	pCSBoids->Bind();
	pCSBoids->Dispatch();
	boiduav.Unbind();

	if (state == 1)
	{
		// in
		texsrv1.Bind();

		// out
		tgt2->Clear();
		texuav2.Bind();

		pCSTrails->Bind();
		//pCSTrails->Dispatch();

		texsrv1.Unbind();
		texuav2.Unbind();

		state = 2;
	}
	else
	{
		// in
		texsrv2.Bind();

		// out
		tgt1->Clear();
		texuav1.Bind();

		pCSTrails->Bind();
		//pCSTrails->Dispatch();

		texsrv2.Unbind();
		texuav1.Unbind();

		state = 1;
	}
}

void Boids2D::OnRender()
{
	if (state == 1)
	{
		tgt1->DrawToBackBuffer();
	}
	else
	{
		tgt2->DrawToBackBuffer();
	}

	DXDevice::GetContext().VSSetShaderResources(boidSlot, 1, pBoidVSSRV.GetAddressOf());

	pPS->Bind();
	pVS->Bind();
	pVB.Bind();
	pVB.Draw(boidCount);


	ID3D11ShaderResourceView* const srv[1] = { nullptr };
	DXDevice::GetContext().VSSetShaderResources(boidSlot, 1, srv);
}

void Boids2D::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
	input.OnApplicationEvent(event);
	//if (event.GetType() == EventType::WindowResize)
	//{
	//}
}

void Boids2D::OnInputEvent(InputEvent& event)
{
	input.OnInputEvent(event);
}

void Boids2D::OnImgui()
{
	if (ImGui::Button("Restart")) InitialiseCS();

	ImGui::DragInt("Diffuse radius", &controlBuffer.data.diffuseRadius, 1.0f, 0, 10);
	ImGui::DragFloat("Diffuse rate", &controlBuffer.data.diffuseRate, 0.005f, 0.0f, 1.0f);
	ImGui::DragFloat("Diffuse fade", &controlBuffer.data.diffuseFade, 0.005f, 0.0f, 1.0f);
	ImGui::Text("Mouse x %u y %u", controlBuffer.data.xMouse, controlBuffer.data.yMouse);
}

void Boids2D::InitialiseGfx()
{
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV"      , 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;

	// triangle
	verticies = {
		{ { -0.5f, -0.5f, 0.0f}, {0.0f, 0.0f} },
		{ { -0.5f,  0.5f, 0.0f}, {0.0f, 1.0f} },
		{ {  0.5f,  0.0f, 0.0f}, {1.0f, 0.5f} },
	};
	indicies = { 0, 1, 2 };

	// square
	//verticies = {
	//	{ { -0.5f, -0.5f, 0.0f}, {0.0f, 0.0f} },
	//	{ { -0.5f,  0.5f, 0.0f}, {0.0f, 1.0f} },
	//	{ {  0.5f,  0.5f, 0.0f}, {1.0f, 1.0f} },
	//	{ {  0.5f, -0.5f, 0.0f}, {1.0f, 0.0f} },
	//};
	//indicies = { 0, 1, 2, 0, 2, 3 };

	pVB.Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies, indicies);

	pVS = VertexShader::CreateFromSourceFile("src\\Boids\\Boids2DVS.hlsl", vsLayout);
	pPS = PixelShader::CreateFromSourceFile("src\\Boids\\Boids2DPS.hlsl");
}

void Boids2D::InitialiseCS(int startPattern)
{
	controlBuffer.slot = 3u;
	controlBuffer.data.time = 0.0f;
	controlBuffer.data.boidCount = boidCount;

	Offscreen::Descriptor desc = {};
	desc.hasDepthBuffer = false;
	desc.unorderedAccess = true;
	desc.DXGIFormat = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	tgt1 = std::make_unique<Offscreen>(desc);
	tgt2 = std::make_unique<Offscreen>(desc);

	controlBuffer.data.xmax = tgt1->GetWidth() - 1;
	controlBuffer.data.ymax = tgt1->GetHeight() - 1;

	pCSTrails = std::make_shared<ComputeShaderFile>("src\\Boids\\Boids2DCS.hlsl");
	pCSTrails->SetThreadGroupCount(tgt1->GetWidth(), tgt1->GetHeight(), 1);

	pCSBoids = std::make_shared<ComputeShaderFile>("src\\Boids\\Boids2DCS.hlsl", "BoidsUpdate");
	pCSBoids->SetThreadGroupCount(1024, 1, 1);
	pCSBoids->SetName("Boids CS");

	texsrv1.Initialise(tgt1->GetSRV());
	texsrv2.Initialise(tgt2->GetSRV());

	texuav1.Initialise(tgt1->GetBuffer());
	texuav2.Initialise(tgt2->GetBuffer());

	struct Boid {
		XMFLOAT3 position;
		float    rotation;
		float    scale;
		XMFLOAT3 color;
	};

	std::vector<Boid> boids;
	boids.reserve(boidCount);
	for (float i = 0.0f; i < boidCount; i++)
	{
		Boid b;
		float r = i / (float)boidCount;

		b.position = { r, r, 0.0f };
		b.rotation = XM_2PI / 12.0f * i;
		b.scale = 0.1f + 0.01f * i;
		b.color = { 0.1f * i, 1.0f, 0.0f };

		boids.push_back(b);
	}

	boidBuffer.InitBuffer(boids, false, D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = boidCount;
	srvDesc.BufferEx.Flags = 0;

	HRESULT hr = DXDevice::GetDevice().CreateShaderResourceView(boidBuffer.GetPtr(), &srvDesc, &pBoidVSSRV);

	E2_ASSERT_HR(hr, "CreateShaderResourceView failed for boid");

	boiduav.Initialise(boidBuffer.Get(), boidCount);
	boiduav.SetSlot(boidSlot);
}
