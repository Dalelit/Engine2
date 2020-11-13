#include "pch.h"
#include "Skybox.h"
#include "SurfaceLoader.h"

namespace Engine2
{
	bool Skybox::Initialise(const std::string& directory)
	{
		HRESULT hr;

		InitialiseTexture(directory);

		// create the depth stencil settings

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;

		hr = DXDevice::GetDevice().CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);

		if (FAILED(hr)) { status = "CreateDepthStencilState failed"; return false; }

		// rasterizer state
		D3D11_RASTERIZER_DESC rsDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = DXDevice::GetDevice().CreateRasterizerState(&rsDesc, &pRasterizerState);

		if (FAILED(hr)) { status = "CreateRasterizerState failed"; return false; }

		// create the vertex buffer
		constexpr float dim = 1.0f;
		using Vertex = DirectX::XMFLOAT3;
		std::vector<Vertex> verticies = {
			{-dim, dim, -dim}, {-dim, dim, dim}, {dim, dim, dim}, {dim, dim, -dim},
			{-dim, -dim, -dim}, {-dim, -dim, dim}, {dim, -dim, dim}, {dim, -dim, -dim},
		};
		std::vector<uint32_t> indicies = {
			0,1,2, 0,2,3,
			4,6,5, 4,7,6,
			4,0,3, 4,3,7,
			7,3,2, 7,2,6,
			6,2,1, 6,1,5,
			5,1,0, 5,0,4,
		};
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		vertexBuffer = std::make_unique<MeshTriangleIndexList<Vertex>>(verticies, indicies);

		vertexShader = std::make_unique<VertexShaderFile>(Config::directories["EngineShaderSourceDir"] + "SkyboxVS.hlsl", layout);
		
		pixelShader = std::make_unique<PixelShaderFile>(Config::directories["EngineShaderSourceDir"] + "SkyboxPS.hlsl");

		status = "";
		SetActive();
		return true;
	}

	bool Skybox::InitialiseTexture(const std::string& directory)
	{
		path = directory;

		//load 6 files
		std::string fileType = ".jpg";
		std::vector<std::string> names = { "right", "left", "top", "bottom", "front", "back" };
		std::vector<std::shared_ptr<Surface>> surfaces;
		surfaces.reserve(6);
		for (int i = 0; i < 6; i++)
		{
			surfaces.push_back(SurfaceLoader::LoadSurface(directory + "\\" + names[i] + fileType));
			if (!surfaces.back()) { status = SurfaceLoader::LastResult; return false; }
		}

		// create the texture

		D3D11_TEXTURE2D_DESC1 texDesc = {};
		texDesc.Width = surfaces[0]->GetWidth();
		texDesc.Height = surfaces[0]->GetHeight();
		texDesc.MipLevels = 1u;
		texDesc.Format = SurfaceLoader::Format;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		// cube map specifics
		texDesc.ArraySize = 6u;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// create the texture view

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		texture = std::make_unique<Texture>(slot, surfaces, texDesc, srvDesc);

		texture->SetSampler(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP);

		return true;
	}


	void Skybox::Bind()
	{
		// cube texture
		texture->Bind();

		// pixel shader
		pixelShader->Bind();

		// vertex shader - assumes the camera transform is already bound in slot 0... so not binding a const buffer
		vertexShader->Bind();
		
		// vertex indexed buffer
		vertexBuffer->Bind();
	}

	void Skybox::Draw()
	{
		// stencil depth first
		DXDevice::GetContext().OMSetDepthStencilState(pDepthStencilState.Get(), 0);

		// rasterizer - front face cull
		DXDevice::GetContext().RSSetState(pRasterizerState.Get());

		// render target - back buffer
		vertexBuffer->Draw();

		DXDevice::Get().SetDefaultDepthStencilState();
		DXDevice::Get().SetDefaultRenderState();
	}

	void Skybox::OnImgui()
	{
		if (ImGui::TreeNode("Skybox"))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::Text("Status: %s", status.c_str());
			ImGui::Text("Path: %s", path.c_str());
			if (texture) texture->OnImgui();
			if (vertexShader) vertexShader->OnImgui();
			if (pixelShader) pixelShader->OnImgui();
			if (vertexBuffer) vertexBuffer->OnImgui();
			ImGui::TreePop();
		}
	}
}
