#include "pch.h"
#include "AntSlime.h"


using namespace Engine2;
using namespace DirectX;

AntSlime::AntSlime() : Layer("AntSlime"), tex1Gizmo(0), tex2Gizmo(1), controlBuffer(3)
{
	Initialise();
}

void AntSlime::OnUpdate(float dt)
{
	if (pause) return;

	controlBuffer.data.time += dt;
	controlBuffer.data.deltaTime = dt;
	controlBuffer.UpdateBuffer();
	controlBuffer.Bind();

	antuav1.Bind();

	if (state == 0)
	{
		texsrv1.Bind();
		tex2->Unbind();
		texuav2.Bind();
		pComputeShader->Bind();
		pComputeShader->Dispatch();

		pComputeShaderAnts->Bind();
		pComputeShaderAnts->Dispatch();

		texuav2.Unbind();

		state = 1;
	}
	else
	{
		texsrv2.Bind();
		tex1->Unbind();
		texuav1.Bind();
		pComputeShader->Bind();
		pComputeShader->Dispatch();

		pComputeShaderAnts->Bind();
		pComputeShaderAnts->Dispatch();

		texuav1.Unbind();

		state = 0;
	}
}

void AntSlime::OnRender()
{
	tex1->Bind();
	//tex2->Bind();
	
	tex1Gizmo.RenderTexture();
	//tex2Gizmo.RenderTexture();
}

void AntSlime::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
}

void AntSlime::OnImgui()
{
	if (ImGui::DragInt("Number of ants", &antCount, 10)) if (antCount < 1) antCount = 1;
	if (ImGui::DragInt("Types 1-2-3", &antTypes, 1, 1, 3)) if (antTypes < 1) antTypes = 1; else if (antTypes > 3) antTypes = 3;
	if (ImGui::Button("All")) Initialise(0);
	ImGui::SameLine();
	if (ImGui::Button("Ring")) Initialise(1);
	ImGui::SameLine();
	if (ImGui::Button("Circle")) Initialise(2);
	ImGui::Checkbox("Pause", &pause);
	ImGui::Separator();
	ImGui::DragFloat("Location range", &controlBuffer.data.locationRange, 0.25f);
	ImGui::DragFloat("Diffuse Loss", &controlBuffer.data.diffuseLoss, 0.005f, 0.0f, 1.0f);
	ImGui::DragFloat("Diffuse Fade", &controlBuffer.data.diffuseFade, 0.005f, 0.0f, 1.0f);
	float degrees = Math::RadToDeg(controlBuffer.data.senseAngle);
	if (ImGui::DragFloat("Sense Angle", &degrees, 0.5f)) controlBuffer.data.senseAngle = Math::DegToRad(degrees);
	ImGui::DragFloat("Sense Range", &controlBuffer.data.senseRange, 0.005f);
	if (ImGui::DragInt("Sense Radius", &controlBuffer.data.senseRadius, 1)) controlBuffer.data.senseRadius = max(1, controlBuffer.data.senseRadius);
	ImGui::DragFloat("Replusion", &controlBuffer.data.replusion, 0.05f);
	ImGui::DragFloat("Speed", &controlBuffer.data.speed, 0.05f);
	ImGui::DragFloat("Steering Strength", &controlBuffer.data.steeringStrength, 0.05f);
	ImGui::DragFloat("Boundary Range", &controlBuffer.data.boundaryRange, 0.05f);

	if (ImGui::TreeNode("Tex1"))
	{
		tex1->OnImgui();
		tex1Gizmo.OnImgui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Tex2"))
	{
		tex2->OnImgui();
		tex2Gizmo.OnImgui();
		ImGui::TreePop();
	}
	pComputeShader->OnImgui();
	pComputeShaderAnts->OnImgui();
}

void AntSlime::Initialise(int startPattern)
{
	using SurfaceType = Engine2::Surface2D<DirectX::XMFLOAT4>;

	UINT w = 1200;
	UINT h = 800;

	controlBuffer.data.xmax = (float)w;
	controlBuffer.data.ymax = (float)h;

	SurfaceType surface1(w, h);
	SurfaceType surface2(w, h);

	surface1.Clear();
	surface2.Clear();

	tex1 = std::make_unique<Texture>(0, surface1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, true);
	tex1->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);

	tex2 = std::make_unique<Texture>(1, surface2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, true);
	tex2->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);

	tex1Gizmo.SetLeftTop(-0.5f, 1.0f);
	tex2Gizmo.SetLeftTop(-0.5f, 0.0f);

	float aspectRatio = (float)w / (float)h / DXDevice::Get().GetAspectRatio();
	float relH = 2.0f;
	tex1Gizmo.SetWidthHeight(relH * aspectRatio, relH);
	tex2Gizmo.SetWidthHeight(relH * aspectRatio, relH);
	tex2Gizmo.SetActive(false);

	//pComputeShader = ComputeShader::CreateFromSourceFile("Assets\\Shaders\\AntSlime.hlsl");
	pComputeShader = std::make_shared<ComputeShaderFile>("Assets\\Shaders\\AntSlime.hlsl");
	pComputeShader->SetThreadGroupCount(w, h, 1);
	pComputeShader->SetName("Environment");
	pComputeShader->AutoUpdate();

	//pComputeShaderAnts = ComputeShader::CreateFromSourceFile("Assets\\Shaders\\AntSlime.hlsl", "UpdateAnts");
	pComputeShaderAnts = std::make_shared<ComputeShaderFile>("Assets\\Shaders\\AntSlime.hlsl", "UpdateAnts");
	pComputeShaderAnts->SetThreadGroupCount(1024, 1, 1);
	pComputeShaderAnts->SetName("Ants");
	pComputeShaderAnts->AutoUpdate();


	texsrv1.Initialise(tex1->GetSRV());
	texsrv2.Initialise(tex2->GetSRV());

	texuav1.Initialise(tex1->GetBuffer());
	texuav2.Initialise(tex2->GetBuffer());

	////

	struct Ant
	{
		int32_t type;
		DirectX::XMFLOAT2 position;
		float angle;
	};

	std::vector<Ant> ant1data;

	float xmax = (float)w;
	float ymax = (float)h;

	ant1data.reserve(antCount);
	Util::Random rngPos;
	Util::Random rngVel(-1.0f, 1.0f);
	float xmid = xmax * 0.5f;
	float ymid = ymax * 0.5f;
	float radius = min(xmax, ymax) / 2.0f * 0.9f;
	Ant a;

	for (int i = 0; i < antCount; i++)
	{
		a.type = i % antTypes;

		if (startPattern == 0)
		{
			a.position = { rngPos.Next() * w , rngPos.Next() * h };
			a.angle = atan2f(ymid - a.position.y, xmid - a.position.x);
		}
		else if (startPattern == 1)
		{
			a.angle = rngVel.Next() * XM_2PI;
			a.position = { xmid - cosf(a.angle) * radius, ymid - sinf(a.angle) * radius };
		}
		else // if (startPattern == 2)
		{
		a.angle = rngVel.Next() * XM_2PI;
		float r = radius * rngPos.Next();
		a.position = { xmid - cosf(a.angle) * r, ymid - sinf(a.angle) * r };
		}

		ant1data.emplace_back(a);
	}

	ants1.InitBuffer<Ant>(ant1data, false, D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);
	antuav1.Initialise(ants1.Get(), antCount);
	antuav1.SetSlot(1);
}
