#pragma once

#include "pch.h"
#include "Common.h"
#include "DXDevice.h"
#include "Resources.h"

namespace Engine2
{
	class Shader : public Bindable
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void OnImgui();

		inline std::string& GetName() { return name; }

	protected:
		std::string name;
		std::string info;
	};

	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> VertexShaderLayoutDesc;

	// Helpers to do some simpler vertex only layouts
	namespace VertexLayoutSimple
	{
		struct VertexShaderLayoutElement
		{
			std::string name;
			DXGI_FORMAT format;
		};

		typedef std::vector<VertexShaderLayoutElement> VertexShaderLayout;

		VertexShaderLayoutDesc ToDescriptor(VertexShaderLayout& layout);
	}

	/////////////////// vertex shader ///////////////////

	class VertexShader : public Shader
	{
	public:
		VertexShader(ID3DBlob& shaderBlob, VertexShaderLayoutDesc& layout, std::string info);
		void Bind();
		void Unbind();

		static std::shared_ptr<VertexShader> CreateFromString(const std::string& src, VertexShaderLayoutDesc& layout, const std::string entryPoint = "main", const std::string target = "vs_5_0");
		static std::shared_ptr<VertexShader> CreateFromCompiledFile(const std::string& filename, VertexShaderLayoutDesc& layout);
		static std::shared_ptr<VertexShader> CreateFromSourceFile(const std::string& filename, VertexShaderLayoutDesc& layout, const std::string entryPoint = "main", const std::string target = "vs_5_0");

	protected:
		VertexShader() = default;
		wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
	};

	class VertexShaderFile : public VertexShader
	{
	public:
		VertexShaderFile(const std::string& filename, VertexShaderLayoutDesc& layout, const std::string entryPoint = "main", const std::string target = "vs_5_0");
		
		void Bind() { if (autoReload) Reload(); VertexShader::Bind(); }
		void Reload(); // only reloads if the file has changed
		inline bool IsValid() { return (pVertexShader && pInputLayout); }
		void OnImgui();

	protected:
		std::string filename;
		std::string entryPoint;
		std::string target;
		VertexShaderLayoutDesc layout;
		FileWatcher fileWatcher;
		std::string status;
		bool autoReload = false;

		void Load();
	};

	/////////////////// pixel shader ///////////////////

	class PixelShader : public Shader
	{
	public:
		PixelShader(ID3DBlob& shaderBlob, std::string info);
		void Bind();
		void Unbind();

		static std::shared_ptr<PixelShader> CreateFromString(const std::string& src, const std::string entryPoint = "main", const std::string target = "ps_5_0");
		static std::shared_ptr<PixelShader> CreateFromCompiledFile(const std::string& filename);
		static std::shared_ptr<PixelShader> CreateFromSourceFile(const std::string& filename, const std::string entryPoint = "main", const std::string target = "ps_5_0");

	protected:
		PixelShader() = default;
		wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	};

	class PixelShaderFile : public PixelShader
	{
	public:
		PixelShaderFile(const std::string& filename, const std::string entryPoint = "main", const std::string target = "ps_5_0");

		void Bind() { if (autoReload) Reload(); PixelShader::Bind(); }
		void Reload(); // only reloads if the file has changed
		inline bool IsValid() { return (pPixelShader); }
		void OnImgui();

	protected:
		std::string filename;
		std::string entryPoint;
		std::string target;
		FileWatcher fileWatcher;
		std::string status;
		bool autoReload = false;

		void Load();
	};

	/////////////////// geometry shader ///////////////////

	class GeometryShader : public Shader
	{
	public:
		GeometryShader(ID3DBlob& shaderBlob, std::string info);
		void Bind();
		void Unbind();

		static std::shared_ptr<GeometryShader> CreateFromSourceFile(const std::string& filename, const std::string entryPoint = "main", const std::string target = "gs_5_0");

	protected:
		GeometryShader() = default;
		wrl::ComPtr<ID3D11GeometryShader> pGeometryShader = nullptr;
	};

}