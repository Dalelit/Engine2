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
		virtual void Unbind() = 0;
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
		void Unbind();

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
		void Unbind();

		static std::shared_ptr<PixelShader> CreateFromString(std::string& src, std::string entryPoint = "main", std::string target = "ps_5_0");
		static std::shared_ptr<PixelShader> CreateFromCompiledFile(std::string& filename);
		static std::shared_ptr<PixelShader> CreateFromSourceFile(std::string& filename, std::string entryPoint = "main", std::string target = "ps_5_0");

	protected:
		wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	};

	/////////////////// geometry shader ///////////////////

	class GeometryShader : public Shader
	{
	public:
		GeometryShader(ID3DBlob& shaderBlob, std::string name);
		void Bind();
		void Unbind();

		static std::shared_ptr<GeometryShader> CreateFromSourceFile(std::string& filename, std::string entryPoint = "main", std::string target = "gs_5_0");

	protected:
		wrl::ComPtr<ID3D11GeometryShader> pGeometryShader = nullptr;
	};

	/////////////////// dynamic shader wrapper ///////////////////

	class VertexShaderDynamic : public Shader
	{
	public:
		// wrap an existing shader
		VertexShaderDynamic(std::string filename, VertexShaderLayout& layout, std::shared_ptr<VertexShader> shader) :
			shader(shader), fileWatcher(filename), layout(layout)
		{}

		// create and wrap a shader
		VertexShaderDynamic(std::string filename, VertexShaderLayout& layout) :
			fileWatcher(filename), layout(layout)
		{
			shader = VertexShader::CreateFromSourceFile(filename, layout);
			E2_ASSERT(shader, "VertexShader::CreateFromSourceFile returned null");
		}

		inline void Bind()
		{
			if (fileWatcher.Check())
			{
				auto newShader = VertexShader::CreateFromSourceFile(fileWatcher.GetFilename(), layout);
				if (newShader) shader = newShader;
			}

			shader->Bind();
		}

		inline void Unbind() { shader->Unbind(); }

		inline void OnImgui()
		{
			ImGui::Text("Dynamic file: %s", fileWatcher.GetFilename().c_str());
			shader->OnImgui();
		}

	protected:
		std::shared_ptr<VertexShader> shader;
		FileWatcher fileWatcher;
		VertexShaderLayout layout;
	};

	class PixelShaderDynamic : public Shader
	{
	public:
		// wrap an existing shader
		PixelShaderDynamic(std::string filename, std::shared_ptr<PixelShader> shader) :
			shader(shader), fileWatcher(filename)
		{}

		// create and wrap a shader
		PixelShaderDynamic(std::string filename) :
			fileWatcher(filename)
		{
			shader = PixelShader::CreateFromSourceFile(filename);
			E2_ASSERT(shader, "PixelShader::CreateFromSourceFile returned null");
		}

		inline void Bind()
		{
			if (fileWatcher.Check())
			{
				auto newShader = PixelShader::CreateFromSourceFile(fileWatcher.GetFilename());
				if (newShader) shader = newShader;
			}
			shader->Bind();
		}

		inline void Unbind() { shader->Unbind(); }

		inline void OnImgui()
		{
			ImGui::Text("Dynamic file: %s", fileWatcher.GetFilename().c_str());
			shader->OnImgui();
		}

	protected:
		std::shared_ptr<PixelShader> shader;
		FileWatcher fileWatcher;
	};

	class GeometryShaderDynamic : public Shader
	{
	public:
		// wrap an existing shader
		GeometryShaderDynamic(std::string filename, std::shared_ptr<GeometryShader> shader) :
			shader(shader), fileWatcher(filename)
		{}

		// create and wrap a shader
		GeometryShaderDynamic(std::string filename) :
			fileWatcher(filename)
		{
			shader = GeometryShader::CreateFromSourceFile(filename);
			E2_ASSERT(shader, "GeometryShader::CreateFromSourceFile returned null");
		}

		inline void Bind()
		{
			if (fileWatcher.Check())
			{
				auto newShader = GeometryShader::CreateFromSourceFile(fileWatcher.GetFilename());
				if (newShader) shader = newShader;
			}
			shader->Bind();
		}

		inline void Unbind() { shader->Unbind(); }

		inline void OnImgui()
		{
			ImGui::Text("Dynamic file: %s", fileWatcher.GetFilename().c_str());
			shader->OnImgui();
		}

	protected:
		std::shared_ptr<GeometryShader> shader;
		FileWatcher fileWatcher;
	};
}