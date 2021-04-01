#include "pch.h"
#include "CSTest.h"


using namespace Engine2;
using namespace DirectX;

CSTest::CSTest() : Layer("CSTest"), tex1Gizmo(0), tex2Gizmo(1)
{
	using SurfaceType = Engine2::Surface2D<DirectX::XMFLOAT4>;

	UINT w = 200;
	UINT h = 100;

	SurfaceType surface1(w, h);
	SurfaceType surface2(w, h);

	XMFLOAT4 on = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 off = { 0.0f, 1.0f, 0.0f, 1.0f };

	XMFLOAT4* current = surface1.begin();

	for (UINT y = 0; y < h; y++)
		for (UINT x = 0; x < w; x++)
		{
			if (x == 0 || y == 0 || x == w-1 || y == h-1) *current = on;
			else if (x % 10 < 5 && y % 10 < 5) *current = on;
			else *current = off;

			current++;
		}

	tex1 = std::make_unique<Texture>(0, surface1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, true);
	tex1->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);

	//

	current = surface2.begin();

	for (UINT y = 0; y < h; y++)
		for (UINT x = 0; x < w; x++)
		{
			*current = on;
			current++;
		}

	tex2 = std::make_unique<Texture>(1, surface2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, true);
	tex2->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);

	//

	tex1Gizmo.SetLeftTop(0.0f, 0.0f);

	pComputeShader = ComputeShader::CreateFromSourceFile("Assets\\Shaders\\CSTest1.hlsl");
	pComputeShader->SetThreadGroupCount(w, h, 1);

	texsrv1.Initialise(tex1->GetSRV());
	texsrv2.Initialise(tex2->GetSRV());

	texuav1.Initialise(tex1->GetBuffer());
	texuav2.Initialise(tex2->GetBuffer());
}

void CSTest::OnUpdate(float dt)
{
	if (state == 0)
	{
		texsrv1.Bind();
		tex2->Unbind();
		texuav2.Bind();
		pComputeShader->Bind();
		pComputeShader->Dispatch();
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
		texuav1.Unbind();

		state = 0;
	}
}

void CSTest::OnRender()
{
	tex1->Bind();
	tex2->Bind();
	
	tex1Gizmo.RenderTexture();
	tex2Gizmo.RenderTexture();
}

void CSTest::OnApplicationEvent(Engine2::ApplicationEvent& event)
{
}

void CSTest::OnImgui()
{

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
}
