#pragma once

#include "pch.h"
#include "Common.h"
#include "DXDevice.h"

namespace Engine2
{
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		void OnImgui();
		void Reload();

		void LoadFromFile(const std::string& filename); // .cso
		void CompileFromFile(const std::string& filename, const std::string& entryPoint, const std::string& target); // .hlsl
		void CompileFromSource(const std::string& source, const std::string& entryPoint, const std::string& target);

		void SetName(const std::string& name) { mName = name; }

		virtual bool IsValid() = 0;

	protected:
		std::string mName;
		std::string mFilename;
		std::string mEntryPoint;
		std::string mTarget;

		void LoadFromFile(); // .cso
		void CompileFromFile(); // .hlsl

		virtual void Initialise(const wrl::ComPtr<ID3DBlob>& pBlob) = 0;
	};

	class VertexShader : public Shader
	{
	public:
		using VertexShaderLayoutDesc = std::vector<D3D11_INPUT_ELEMENT_DESC>;

		void Bind()
		{
			DXDevice::GetContext().VSSetShader(pVertexShader.Get(), nullptr, 0u);
			DXDevice::GetContext().IASetInputLayout(pInputLayout.Get());
		}

		void Unbind()
		{
			DXDevice::GetContext().VSSetShader(nullptr, nullptr, 0u);
			DXDevice::GetContext().IASetInputLayout(nullptr);
		}

		void LoadFromFile(const std::string& filename, const VertexShaderLayoutDesc& layout)
		{
			mLayout = layout;
			Shader::LoadFromFile(filename);
		}

		void CompileFromFile(const std::string& filename, const VertexShaderLayoutDesc& layout, const std::string& entryPoint = "main", const std::string& target = "vs_5_0")
		{
			mLayout = layout;
			Shader::CompileFromFile(filename, entryPoint, target);
		}

		void CompileFromSource(const std::string& source, const VertexShaderLayoutDesc& layout, const std::string& entryPoint = "main", const std::string& target = "vs_5_0")
		{
			mLayout = layout;
			Shader::CompileFromSource(source, entryPoint, target);
		}

		bool IsValid() { return pVertexShader != nullptr; }

		static std::shared_ptr<VertexShader> MakeShared() { return std::make_shared<VertexShader>(); }

	protected:
		VertexShaderLayoutDesc mLayout;
		wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;

		void Initialise(const wrl::ComPtr<ID3DBlob>& pBlob);
	};

	class PixelShader : public Shader
	{
	public:
		void Bind() { DXDevice::GetContext().PSSetShader(pPixelShader.Get(), nullptr, 0u); }
		void Unbind() { DXDevice::GetContext().PSSetShader(nullptr, nullptr, 0u); }

		void LoadFromFile(const std::string& filename)
		{
			Shader::LoadFromFile(filename);
		}

		void CompileFromFile(const std::string& filename, const std::string& entryPoint = "main", const std::string& target = "ps_5_0")
		{
			Shader::CompileFromFile(filename, entryPoint, target);
		}

		void CompileFromSource(const std::string& source, const std::string& entryPoint = "main", const std::string& target = "ps_5_0")
		{
			Shader::CompileFromSource(source, entryPoint, target);
		}

		bool IsValid() { return pPixelShader != nullptr; }

		static std::shared_ptr<PixelShader> MakeShared() { return std::make_shared<PixelShader>(); }

	protected:
		wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;

		void Initialise(const wrl::ComPtr<ID3DBlob>& pBlob);
	};

	class GeometryShader : public Shader
	{
	public:
		void Bind() { DXDevice::GetContext().GSSetShader(pGeometryShader.Get(), nullptr, 0u); }
		void Unbind() { DXDevice::GetContext().GSSetShader(nullptr, nullptr, 0u); }

		void LoadFromFile(const std::string& filename)
		{
			Shader::LoadFromFile(filename);
		}

		void CompileFromFile(const std::string& filename, const std::string& entryPoint = "main", const std::string& target = "gs_5_0")
		{
			Shader::CompileFromFile(filename, entryPoint, target);
		}

		void CompileFromSource(const std::string& source, const std::string& entryPoint = "main", const std::string& target = "gs_5_0")
		{
			Shader::CompileFromSource(source, entryPoint, target);
		}

		bool IsValid() { return pGeometryShader != nullptr; }

		static std::shared_ptr<GeometryShader> MakeShared() { return std::make_shared<GeometryShader>(); }

	protected:
		wrl::ComPtr<ID3D11GeometryShader> pGeometryShader = nullptr;

		void Initialise(const wrl::ComPtr<ID3DBlob>& pBlob);
	};

	class ComputeShader : public Shader
	{
	public:
		void Dispatch() { DXDevice::GetContext().Dispatch(threadGroupCount[0], threadGroupCount[1], threadGroupCount[2]); }

		void Bind() { DXDevice::GetContext().CSSetShader(pComputeShader.Get(), nullptr, 0u); }
		void Unbind() { DXDevice::GetContext().CSSetShader(nullptr, nullptr, 0u); }

		void SetThreadGroupCount(UINT x, UINT y, UINT z) { threadGroupCount[0] = x; threadGroupCount[1] = y; threadGroupCount[2] = z; }

		void LoadFromFile(const std::string& filename)
		{
			Shader::LoadFromFile(filename);
		}

		void CompileFromFile(const std::string& filename, const std::string& entryPoint = "main", const std::string& target = "cs_5_0")
		{
			Shader::CompileFromFile(filename, entryPoint, target);
		}

		void CompileFromSource(const std::string& source, const std::string& entryPoint = "main", const std::string& target = "cs_5_0")
		{
			Shader::CompileFromSource(source, entryPoint, target);
		}

		bool IsValid() { return pComputeShader != nullptr; }

		static std::shared_ptr<ComputeShader> MakeShared() { return std::make_shared<ComputeShader>(); }

	protected:
		wrl::ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;
		UINT threadGroupCount[3] = { 1,1,1 };

		void Initialise(const wrl::ComPtr<ID3DBlob>& pBlob);
	};
}