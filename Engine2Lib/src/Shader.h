#pragma once

#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() = 0;

	protected:

	};

	struct VertexShaderLayoutElement
	{
		std::string name;
		DXGI_FORMAT format;
	};

	typedef std::vector<VertexShaderLayoutElement> VertexShaderLayout;

	class VertexShader : public Shader
	{
	public:
		VertexShader(ID3DBlob& shaderBlob, VertexShaderLayout& layout);
		void Bind();

		static std::shared_ptr<VertexShader> CreateFromString(std::string& src, VertexShaderLayout& layout, std::string entryPoint = "main", std::string target = "vs_5_0");

	protected:

		wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader(ID3DBlob& shaderBlob);
		void Bind();

		static std::shared_ptr<PixelShader> CreateFromString(std::string& src, std::string entryPoint = "main", std::string target = "ps_5_0");

	protected:
		wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	};
}