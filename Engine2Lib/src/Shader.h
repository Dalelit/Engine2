#pragma once

#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	class Shader
	{
	public:
		Shader(std::string name) : name(name) {}
		virtual ~Shader() = default;
		virtual void Bind() = 0;
		virtual void OnImgui();

	protected:
		std::string name;
		std::string source;
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
		VertexShader(ID3DBlob& shaderBlob, VertexShaderLayout& layout, std::string name);
		void Bind();

		static std::shared_ptr<VertexShader> CreateFromString(std::string& src, VertexShaderLayout& layout, std::string entryPoint = "main", std::string target = "vs_5_0");
		static std::shared_ptr<VertexShader> CreateFromCompiledFile(std::string& filename, VertexShaderLayout& layout);

	protected:
		wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
		std::string name;
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader(ID3DBlob& shaderBlob, std::string name);
		void Bind();

		static std::shared_ptr<PixelShader> CreateFromString(std::string& src, std::string entryPoint = "main", std::string target = "ps_5_0");
		static std::shared_ptr<PixelShader> CreateFromCompiledFile(std::string& filename);

	protected:
		wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	};
}