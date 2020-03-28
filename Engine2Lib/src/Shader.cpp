#include "pch.h"
#include "Common.h"
#include "Shader.h"

namespace Engine2
{
	void Shader::OnImgui()
	{
		ImGui::Text(name.c_str());
		ImGui::Text(source.c_str());
	}

	void VertexShader::Bind()
	{
		Engine::GetContext().VSSetShader(pVertexShader.Get(), nullptr, 0u);
		Engine::GetContext().IASetInputLayout(pInputLayout.Get());
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromString(std::string& src, VertexShaderLayout& layout, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(src.data(), src.length(), NULL, NULL, NULL, entryPoint.data(), target.data(), 0, 0, &pBlob, &pErrBlob);

		E2_ASSERT_HR(hr, "VertexShader D3DCompile failed");

		auto shader = std::make_shared<VertexShader>(*pBlob.Get(), layout, "Vertex Shader, string: " + entryPoint + " " + target);

		shader->source = src;

		return shader;
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromCompiledFile(std::string& filename, VertexShaderLayout& layout)
	{
		wrl::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = D3DReadFileToBlob(Util::ToWString(filename).c_str(), &pBlob);

		E2_ASSERT_HR(hr, "VertexShader D3DReadFileToBlob failed");

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, "Vertex Shader, compiled file: " + filename);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromSourceFile(std::string& filename, VertexShaderLayout& layout, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr)) return nullptr;

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, "Vertex Shader, source file: " + entryPoint + " " + target);
	}

	VertexShader::VertexShader(ID3DBlob& shaderBlob, VertexShaderLayout& layout, std::string name) : Shader(name)
	{
		HRESULT hr;
		
		hr = Engine::GetDevice().CreateVertexShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pVertexShader);

		E2_ASSERT_HR(hr, "VertexShader CreateVertexShader failed");

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc(layout.size());

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

		hr = Engine::GetDevice().CreateInputLayout(
			inputDesc.data(),
			(UINT)inputDesc.size(),
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			&pInputLayout);

		E2_ASSERT_HR(hr, "VertexShader CreateInputLayout failed");
	}

	PixelShader::PixelShader(ID3DBlob& shaderBlob, std::string name) : Shader(name)
	{
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

	std::shared_ptr<PixelShader> PixelShader::CreateFromString(std::string& src, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(src.data(), src.length(), NULL, NULL, NULL, entryPoint.data(), target.data(), 0, 0, &pBlob, &pErrBlob);

		E2_ASSERT_HR(hr, "PixelShader D3DCompile failed");

		auto shader = std::make_shared<PixelShader>(*pBlob.Get(), "Pixel Shader, string: " + entryPoint + " " + target);

		shader->source = src;

		return shader;
	}
	std::shared_ptr<PixelShader> PixelShader::CreateFromCompiledFile(std::string& filename)
	{
		wrl::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = D3DReadFileToBlob(Util::ToWString(filename).c_str(), &pBlob);

		E2_ASSERT_HR(hr, "PixelShader D3DReadFileToBlob failed");

		return std::make_shared<PixelShader>(*pBlob.Get(), "Pixel Shader, compiled file: " + filename);
	}
	std::shared_ptr<PixelShader> PixelShader::CreateFromSourceFile(std::string& filename, std::string entryPoint, std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr)) return nullptr;

		return std::make_shared<PixelShader>(*pBlob.Get(), "Pixel Shader, source file: " + entryPoint + " " + target);
	}
}
