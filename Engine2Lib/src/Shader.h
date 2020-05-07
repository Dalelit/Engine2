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

		void CreateVertexShader(ID3DBlob& shaderBlob, std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const std::string info);
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

	/////////////////// dynamic shader wrapper ///////////////////

	class VertexShaderDynamic : public VertexShader
	{
	public:
		// wrap an existing shader
		VertexShaderDynamic(std::string filename, VertexShaderLayoutDesc& layout, std::shared_ptr<VertexShader> shader) :
			shader(shader), fileWatcher(filename), layout(layout)
		{}

		// create and wrap a shader
		VertexShaderDynamic(std::string filename, VertexShaderLayoutDesc& layout) :
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
			if (ImGui::TreeNode("Dynamic Vertex Shader"))
			{
				ImGui::Text("Watching file: %s", fileWatcher.GetFilename().c_str());
				shader->OnImgui();
				ImGui::TreePop();
			}
		}

	protected:
		std::shared_ptr<VertexShader> shader;
		FileWatcher fileWatcher;
		VertexShaderLayoutDesc layout;
	};

	class PixelShaderDynamic : public PixelShader
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
			if (ImGui::TreeNode("Dynamic Pixel Shader"))
			{
				ImGui::Text("Watching file: %s", fileWatcher.GetFilename().c_str());
				shader->OnImgui();
				ImGui::TreePop();
			}
		}

	protected:
		std::shared_ptr<PixelShader> shader;
		FileWatcher fileWatcher;
	};

	class GeometryShaderDynamic : public GeometryShader
	{
	public:
		// wrap an existing shader
		GeometryShaderDynamic(const std::string filename, std::shared_ptr<GeometryShader> shader) :
			shader(shader), fileWatcher(filename)
		{}

		// create and wrap a shader
		GeometryShaderDynamic(const std::string filename) :
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
			if (ImGui::TreeNode("Dynamic Geometry Shader"))
			{
				ImGui::Text("Watching file: %s", fileWatcher.GetFilename().c_str());
				shader->OnImgui();
				ImGui::TreePop();
			}
		}

	protected:
		std::shared_ptr<GeometryShader> shader;
		FileWatcher fileWatcher;
	};
}