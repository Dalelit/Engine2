#include "pch.h"
#include "Boids2D.h"
#include "Surface.h"
#include "VertexLayout.h"

using namespace Engine2;
using namespace DirectX;

Boids2D::Boids2D() : Layer("Boids2D")
{
	worldCB.slot = 1;
	controlCB.slot = 3u;

	controlCB.data.time = 0.0f;
	controlCB.data.deltaTime = 0.0f;

	InitialiseGfx();
	InitialiseCS();
}

void Boids2D::OnUpdate(float dt)
{
	input.OnUpdate(dt);

	worldCB.CSBind();
	worldCB.GSBind();

	controlCB.data.time += dt;
	controlCB.data.deltaTime = dt;
	controlCB.Bind(); // does the update buffer before binding
	controlCB.GSBind();

	// bind and process buffer for trails
	if (state == 1)
	{
		// in
		texsrv1.Bind();

		// out
		tgt2->Clear();
		texuav2.Bind();

		pCSTrails->Bind();
		pCSTrails->Dispatch();
	}
	else
	{
		// in
		texsrv2.Bind();

		// out
		tgt1->Clear();
		texuav1.Bind();

		pCSTrails->Bind();
		pCSTrails->Dispatch();
	}

	// update boids
	boiduav.Bind();
	pCSBoids->Bind();
	pCSBoids->Dispatch();
	boiduav.Unbind();

	// clean up buffer for trails
	if (state == 1)
	{
		texsrv1.Unbind();
		texuav2.Unbind();
		state = 2;
	}
	else
	{
		texsrv2.Unbind();
		texuav1.Unbind();
		state = 1;
	}

}

void Boids2D::OnRender()
{
	// draw trails
	if (state == 1)
		tgt1->DrawToBackBuffer();
	else
		tgt2->DrawToBackBuffer();

	// bind common resources
	DXDevice::GetContext().VSSetShaderResources(boidSlot, 1, pBoidVSSRV.GetAddressOf());
	DXDevice::GetContext().GSSetShaderResources(boidSlot, 1, pBoidVSSRV.GetAddressOf());
	worldCB.VSBind();
	controlCB.VSBind();

	// draw boids
	pPS->Bind();
	pVS->Bind();
	pVB.Bind();
	pVB.Draw(boidCount);

	// draw sense lines
	if (showSenseLines)
	{
		pVSSenseLines->Bind();
		pPSSenseLines->Bind();
		pGSSenseLines->Bind();
		pVBSenseLines.Bind();
		pVBSenseLines.Draw(boidCount);
		pGSSenseLines->Unbind();
	}

	// draw sense radius
	if (showSenseRadius)
	{
		pPSSense->Bind();
		pVSSense->Bind();
		pVBSense.Bind();
		pVBSense.Draw(boidCount);
	}

	// clean up
	ID3D11ShaderResourceView* const srv[1] = { nullptr };
	DXDevice::GetContext().VSSetShaderResources(boidSlot, 1, srv);
	DXDevice::GetContext().GSSetShaderResources(boidSlot, 1, srv);
}

void Boids2D::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
	input.OnApplicationEvent(event);
	if (event.GetType() == EventType::WindowResize)
	{
		CalcWorldWidth();
	}
}

void Boids2D::OnInputEvent(InputEvent& event)
{
	input.OnInputEvent(event);
}

void Boids2D::OnImgui()
{
	if (ImGui::DragScalar("Boid count", ImGuiDataType_::ImGuiDataType_U32, &boidCount, 1.0f)) { if (boidCount < 1) boidCount = 1; }
	if (ImGui::Button("Restart")) InitialiseCS();
	ImGui::Separator();
	ImGui::Text("Boid");
	ImGui::DragFloat("Speed", &controlCB.data.boidSpeed, 0.05f);
	ImGui::DragFloat("Scale", &controlCB.data.boidScale, 0.05f);
	ImGui::DragFloat("Sense Radius", &controlCB.data.boidSenseRadius, 0.05f);
	ImGui::DragFloat("Centre strength", &controlCB.data.centreStrength, 0.05f);
	ImGui::DragFloat("Direction strength", &controlCB.data.directionStrength, 0.05f);
	ImGui::DragFloat("Repulsion strength", &controlCB.data.repulsionStrength, 0.05f);
	ImGui::Checkbox("Show sense radius", &showSenseRadius);
	ImGui::Checkbox("Show sense lines", &showSenseLines);
	ImGui::Separator();
	if (ImGui::DragFloat("World Height", &worldCB.data.worldDimension.y, 0.05f)) { CalcWorldWidth(); }
	ImGui::Text("World Width %f", worldCB.data.worldDimension.x);
	ImGui::Separator();
	ImGui::DragInt("Diffuse radius", &controlCB.data.diffuseRadius, 1.0f, 0, 10);
	ImGui::DragFloat("Diffuse rate", &controlCB.data.diffuseRate, 0.005f, 0.0f, 1.0f);
	ImGui::DragFloat("Diffuse fade", &controlCB.data.diffuseFade, 0.005f, 0.0f, 1.0f);
	ImGui::Text("Screen %u x %u", worldCB.data.screenDimension.x, worldCB.data.screenDimension.y);
	pCSBoids->OnImgui();
	pCSTrails->OnImgui();
	pVS->OnImgui();
	pPS->OnImgui();
	pVSSense->OnImgui();
	pPSSense->OnImgui();
}

void Boids2D::InitialiseGfx()
{
	CalcWorldWidth();

	struct Vertex {
		XMFLOAT2 position;
		XMFLOAT2 uv;
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV"      , 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// triangle
	std::vector<Vertex> verticiesTri = {
		{ { -1.0f, -1.0f}, {0.0f, 0.0f} },
		{ { -1.0f,  1.0f}, {0.0f, 1.0f} },
		{ {  1.0f,  0.0f}, {1.0f, 0.5f} },
	};
	std::vector<unsigned int> indiciesTri = { 0, 1, 2 };

	// square
	std::vector<Vertex> verticiesSqr = {
		{ { -0.5f, -0.5f}, {0.0f, 0.0f} },
		{ { -0.5f,  0.5f}, {0.0f, 1.0f} },
		{ {  0.5f,  0.5f}, {1.0f, 1.0f} },
		{ {  0.5f, -0.5f}, {1.0f, 0.0f} },
	};
	std::vector<unsigned int> indiciesSqr = { 0, 1, 2, 0, 2, 3 };

	pVB.Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticiesTri, indiciesTri);
	pVS = std::make_shared<VertexShaderFile>("src\\Boids\\Boids2DVS.hlsl", vsLayout);
	pPS = std::make_shared<PixelShaderFile>("src\\Boids\\Boids2DPS.hlsl");

	pVBSense.Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticiesSqr, indiciesSqr);
	pVSSense = std::make_shared<VertexShaderFile>("src\\Boids\\Boids2DVS.hlsl", vsLayout, "sense");
	pPSSense = std::make_shared<PixelShaderFile>("src\\Boids\\Boids2DPS.hlsl", "sense");


	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayoutSenseLines = {
		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// create this just to have a basic vertex buffer
	std::vector<XMFLOAT2> linesPoint = { {0.0f,0.0f} };
	pVBSenseLines.Initialise<XMFLOAT2>(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, linesPoint);

	pVSSenseLines = std::make_shared<VertexShaderFile>("src\\Boids\\Boids2DVS.hlsl", vsLayoutSenseLines, "senseLines");
	pPSSenseLines = std::make_shared<PixelShaderFile>("src\\Boids\\Boids2DPS.hlsl", "senseLines");
	pGSSenseLines = GeometryShader::CreateFromSourceFile("src\\Boids\\Boids2DGS.hlsl");
}

void Boids2D::InitialiseCS(int startPattern)
{
	controlCB.data.time = 0.0f;
	controlCB.data.boidCount = boidCount;

	Offscreen::Descriptor desc = {};
	desc.hasDepthBuffer = false;
	desc.unorderedAccess = true;
	desc.DXGIFormat = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.height = 300;
	desc.width = (int)((float)desc.height * DXDevice::Get().GetAspectRatio());
	tgt1 = std::make_unique<Offscreen>(desc);
	tgt2 = std::make_unique<Offscreen>(desc);

	worldCB.data.screenDimension.x = tgt1->GetWidth() - 1;
	worldCB.data.screenDimension.y = tgt1->GetHeight() - 1;

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
		XMFLOAT2 position;
		XMFLOAT2 direction;
		XMFLOAT3 color;
	};

	std::vector<Boid> boids;
	boids.reserve(boidCount);
	Util::Random rng;
	Util::Random rngColor(0.1f, 1.0f);
	for (float i = 0.0f; i < boidCount; i++)
	{
		Boid b;

		b.position = { rng.Next() * worldCB.data.worldDimension.x, rng.Next() * worldCB.data.worldDimension.y };

		float angle = rng.Next() * XM_2PI;
		b.direction = { cosf(angle), sinf(angle) };
		
		b.color = { rngColor.Next(), rngColor.Next(), rngColor.Next() };

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
