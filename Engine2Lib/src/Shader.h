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
		virtual void OnImgui();

		inline std::string& GetName() { return name; }

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

	/////////////////// vertex shader ///////////////////

	class VertexShader : public Shader
	{
	public:
		VertexShader(ID3DBlob& shaderBlob, VertexShaderLayout& layout, std::string name);
		void Bind();

		static std::shared_ptr<VertexShader> CreateFromString(std::string& src, VertexShaderLayout& layout, std::string entryPoint = "main", std::string target = "vs_5_0");
		static std::shared_ptr<VertexShader> CreateFromCompiledFile(std::string& filename, VertexShaderLayout& layout);
		static std::shared_ptr<VertexShader> CreateFromSourceFile(std::string& filename, VertexShaderLayout& layout, std::string entryPoint = "main", std::string target = "vs_5_0");

	protected:
		wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
	};

	/////////////////// pixel shader ///////////////////

	class PixelShader : public Shader
	{
	public:
		PixelShader(ID3DBlob& shaderBlob, std::string name);
		void Bind();

		static std::shared_ptr<PixelShader> CreateFromString(std::string& src, std::string entryPoint = "main", std::string target = "ps_5_0");
		static std::shared_ptr<PixelShader> CreateFromCompiledFile(std::string& filename);
		static std::shared_ptr<PixelShader> CreateFromSourceFile(std::string& filename, std::string entryPoint = "main", std::string target = "ps_5_0");

	protected:
		wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	};

	/////////////////// dynamic shader wrapper ///////////////////

	template <typename T>
	class ShaderDynamic : public Shader
	{
	public:
		ShaderDynamic(std::string filename, std::shared_ptr<T> shader) :
			fileWatcher(filename), shader(shader)
		{
			this->name = shader->GetName() + " dynamic";
		}

		void Bind()
		{
			if (fileWatcher.Check())
			{
				auto newShader = T::CreateFromSourceFile(fileWatcher.GetFilename());
				if (newShader)
				{
					this->name = shader->GetName() + " dynamic";
					shader = newShader;
				}
			}

			shader->Bind();
		}

		void OnImgui()
		{
			ImGui::Text("Dynamic file: %s", fileWatcher.GetFilename().c_str());
			shader->OnImgui();
		}

	protected:
		std::shared_ptr<T> shader;
		FileWatcher fileWatcher;
	};
}