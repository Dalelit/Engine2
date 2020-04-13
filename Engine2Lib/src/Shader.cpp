#include "pch.h"
#include "Common.h"
#include "Shader.h"

namespace Engine2
{
	void Shader::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text(info.c_str());
			ImGui::TreePop();
		}
	}

	VertexShaderLayoutDesc VertexLayoutSimple::ToDescriptor(VertexShaderLayout& layout)
	{
		VertexShaderLayoutDesc inputDesc(layout.size());

		for (int i = 0; i < layout.size(); i++)
		{
			inputDesc[i].SemanticName = layout[i].name.c_str();
			inputDesc[i].SemanticIndex = 0;
			inputDesc[i].Format = layout[i].format;
			inputDesc[i].InputSlot = 0u;
			inputDesc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			inputDesc[i].InstanceDataStepRate = 0u;
		}

		return inputDesc;
	}

	///////////////// Vertex shaders /////////////////

	VertexShader::VertexShader(ID3DBlob& shaderBlob, VertexShaderLayoutDesc& layout, std::string info)
	{
		this->name = "VertexShader";
		this->info = info;

			HRESULT hr;

		hr = Engine::GetDevice().CreateVertexShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pVertexShader);

		E2_ASSERT_HR(hr, "VertexShader CreateVertexShader failed");

		hr = Engine::GetDevice().CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			&pInputLayout);

		E2_ASSERT_HR(hr, "VertexShader CreateInputLayout failed");
	}

	void VertexShader::Bind()
	{
		Engine::GetContext().VSSetShader(pVertexShader.Get(), nullptr, 0u);
		Engine::GetContext().IASetInputLayout(pInputLayout.Get());
	}

	void VertexShader::Unbind()
	{
		Engine::GetContext().VSSetShader(nullptr, nullptr, 0u);
		Engine::GetContext().IASetInputLayout(nullptr);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromString(std::string& src, VertexShaderLayoutDesc& layout, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(src.data(), src.length(), NULL, NULL, NULL, entryPoint.data(), target.data(), 0, 0, &pBlob, &pErrBlob);

		E2_ASSERT_HR(hr, "VertexShader D3DCompile failed");

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, entryPoint + " " + target + "\nSource string\n" + src);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromCompiledFile(std::string& filename, VertexShaderLayoutDesc& layout)
	{
		wrl::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = D3DReadFileToBlob(Util::ToWString(filename).c_str(), &pBlob);

		E2_ASSERT_HR(hr, "VertexShader D3DReadFileToBlob failed");

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, "Compiled file: " + filename);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromSourceFile(std::string& filename, VertexShaderLayoutDesc& layout, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr)) return nullptr;

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, "Source file: " + filename + "\n" + entryPoint + " " + target);
	}

	///////////////// Pixel shaders /////////////////

	PixelShader::PixelShader(ID3DBlob& shaderBlob, std::string info)
	{
		this->name = "PixelShader";
		this->info = info;

		HRESULT hr = Engine::GetDevice().CreatePixelShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pPixelShader);

		E2_ASSERT_HR(hr, "PixelShader CreatePixelShader failed");
	}

	void PixelShader::Bind()
	{
		Engine::GetContext().PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}

	void PixelShader::Unbind()
	{
		Engine::GetContext().PSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<PixelShader> PixelShader::CreateFromString(std::string& src, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(src.data(), src.length(), NULL, NULL, NULL, entryPoint.data(), target.data(), 0, 0, &pBlob, &pErrBlob);

		E2_ASSERT_HR(hr, "PixelShader D3DCompile failed");

		return std::make_shared<PixelShader>(*pBlob.Get(), entryPoint + " " + target + "\nSource\n" + src);
	}

	std::shared_ptr<PixelShader> PixelShader::CreateFromCompiledFile(std::string& filename)
	{
		wrl::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = D3DReadFileToBlob(Util::ToWString(filename).c_str(), &pBlob);

		E2_ASSERT_HR(hr, "PixelShader D3DReadFileToBlob failed");

		return std::make_shared<PixelShader>(*pBlob.Get(), "Compiled file: " + filename);
	}

	std::shared_ptr<PixelShader> PixelShader::CreateFromSourceFile(std::string& filename, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr)) return nullptr;

		return std::make_shared<PixelShader>(*pBlob.Get(), "Source file: " + filename + "\n"+ entryPoint + " " + target);
	}

	///////////////// Geometry shaders /////////////////


	GeometryShader::GeometryShader(ID3DBlob& shaderBlob, std::string info)
	{
		this->name = "GeometryShader";
		this->info = info;

		HRESULT hr = Engine::GetDevice().CreateGeometryShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pGeometryShader);

		E2_ASSERT_HR(hr, "GeometryShader CreateGeometryShader failed");
	}

	void GeometryShader::Bind()
	{
		Engine::GetContext().GSSetShader(pGeometryShader.Get(), nullptr, 0u);
	}

	void GeometryShader::Unbind()
	{
		Engine::GetContext().GSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<GeometryShader> GeometryShader::CreateFromSourceFile(std::string& filename, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr)) return nullptr;

		return std::make_shared<GeometryShader>(*pBlob.Get(), "Source file: " + filename + "\n" + entryPoint + " " + target);
	}

}
