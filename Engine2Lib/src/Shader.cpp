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

	///////////////// Vertex shaders /////////////////

	VertexShader::VertexShader(ID3DBlob& shaderBlob, VertexShaderLayoutDesc& layout, std::string info)
	{
		this->name = "VertexShader";
		this->info = info;

		HRESULT hr;

		hr = DXDevice::GetDevice().CreateVertexShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pVertexShader);

		E2_ASSERT_HR(hr, "VertexShader CreateVertexShader failed");

		hr = DXDevice::GetDevice().CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			&pInputLayout);

		E2_ASSERT_HR(hr, "VertexShader CreateInputLayout failed");
	}

	void VertexShader::Bind()
	{
		DXDevice::GetContext().VSSetShader(pVertexShader.Get(), nullptr, 0u);
		DXDevice::GetContext().IASetInputLayout(pInputLayout.Get());
	}

	void VertexShader::Unbind()
	{
		DXDevice::GetContext().VSSetShader(nullptr, nullptr, 0u);
		DXDevice::GetContext().IASetInputLayout(nullptr);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromString(const std::string& src, VertexShaderLayoutDesc& layout, const std::string entryPoint, const std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(src.data(), src.length(), NULL, NULL, NULL, entryPoint.data(), target.data(), 0, 0, &pBlob, &pErrBlob);

		E2_ASSERT_HR(hr, "VertexShader D3DCompile failed");

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, entryPoint + " " + target + "\nSource string\n" + src);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromCompiledFile(const std::string& filename, VertexShaderLayoutDesc& layout)
	{
		wrl::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = D3DReadFileToBlob(Util::ToWString(filename).c_str(), &pBlob);

		E2_ASSERT_HR(hr, "VertexShader D3DReadFileToBlob failed");

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, "Compiled file: " + filename);
	}

	std::shared_ptr<VertexShader> VertexShader::CreateFromSourceFile(const std::string& filename, VertexShaderLayoutDesc& layout, const std::string entryPoint, const std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			std::string errorMsg = (char*)pErrBlob->GetBufferPointer();
			Logging::LogError("Vertex shader failed.");
			Logging::LogError(errorMsg);
			return nullptr;
		}

		return std::make_shared<VertexShader>(*pBlob.Get(), layout, "Source file: " + filename + "\n" + entryPoint + " " + target);
	}

	VertexShaderFile::VertexShaderFile(const std::string& filename, VertexShaderLayoutDesc& layout, const std::string entryPoint, const std::string target) :
		filename(filename), layout(layout), entryPoint(entryPoint), target(target), fileWatcher(filename)
	{
		this->name = "VertexShaderFile";
		this->info = "Source file: " + filename + "\n" + entryPoint + " " + target;

		Load();
	}

	void VertexShaderFile::Reload()
	{
		if (fileWatcher.Check())
		{
			pVertexShader.Reset();
			pInputLayout.Reset();
			Load();
		}
	}

	void VertexShaderFile::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text(info.c_str());
			ImGui::Text(status.c_str());
			ImGui::Checkbox("Auto update", &autoReload);
			ImGui::TreePop();
		}
	}

	void VertexShaderFile::Load()
	{
		status = "";

		HRESULT hr;
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			status = "Failed to compile file " + filename;
			return;
		}

		hr = DXDevice::GetDevice().CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVertexShader);

		if (FAILED(hr))
		{
			status = "Failed to CreateVertexShader";
			return;
		}

		hr = DXDevice::GetDevice().CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout);

		if (FAILED(hr))
		{
			status = "Failed to CreateInputLayout";
			return;
		}

		status = "Loaded";
	}

	///////////////// Pixel shaders /////////////////

	PixelShader::PixelShader(ID3DBlob& shaderBlob, const std::string info)
	{
		this->name = "PixelShader";
		this->info = info;

		HRESULT hr = DXDevice::GetDevice().CreatePixelShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pPixelShader);

		E2_ASSERT_HR(hr, "PixelShader CreatePixelShader failed");
	}

	void PixelShader::Bind()
	{
		DXDevice::GetContext().PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}

	void PixelShader::Unbind()
	{
		DXDevice::GetContext().PSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<PixelShader> PixelShader::CreateFromString(const std::string& src, const std::string entryPoint, const std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(src.data(), src.length(), NULL, NULL, NULL, entryPoint.data(), target.data(), 0, 0, &pBlob, &pErrBlob);

		E2_ASSERT_HR(hr, "PixelShader D3DCompile failed");

		return std::make_shared<PixelShader>(*pBlob.Get(), entryPoint + " " + target + "\nSource\n" + src);
	}

	std::shared_ptr<PixelShader> PixelShader::CreateFromCompiledFile(const std::string& filename)
	{
		wrl::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = D3DReadFileToBlob(Util::ToWString(filename).c_str(), &pBlob);

		E2_ASSERT_HR(hr, "PixelShader D3DReadFileToBlob failed");

		return std::make_shared<PixelShader>(*pBlob.Get(), "Compiled file: " + filename);
	}

	std::shared_ptr<PixelShader> PixelShader::CreateFromSourceFile(const std::string& filename, const std::string entryPoint, const std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			std::string errorMsg = (char*)pErrBlob->GetBufferPointer();
			Logging::LogError("Pixel shader failed.");
			Logging::LogError(errorMsg);
			return nullptr;
		}

		return std::make_shared<PixelShader>(*pBlob.Get(), "Source file: " + filename + "\n"+ entryPoint + " " + target);
	}

	PixelShaderFile::PixelShaderFile(const std::string& filename, const std::string entryPoint, const std::string target) :
		filename(filename), entryPoint(entryPoint), target(target), fileWatcher(filename)
	{
		this->name = "PixelShaderFile";
		this->info = "Source file: " + filename + "\n" + entryPoint + " " + target;

		Load();
	}

	void PixelShaderFile::Reload()
	{
		if (fileWatcher.Check())
		{
			pPixelShader.Reset();
			Load();
		}
	}

	void PixelShaderFile::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text(info.c_str());
			ImGui::Text(status.c_str());
			ImGui::Checkbox("Auto update", &autoReload);
			ImGui::TreePop();
		}
	}

	void PixelShaderFile::Load()
	{
		status = "";

		HRESULT hr;
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			status = "Failed to compile file " + filename;
			return;
		}

		hr = DXDevice::GetDevice().CreatePixelShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pPixelShader);

		if (FAILED(hr))
		{
			status = "Failed to CreatePixelShader";
			return;
		}

		status = "Loaded";
	}

	///////////////// Geometry shaders /////////////////


	GeometryShader::GeometryShader(ID3DBlob& shaderBlob, const std::string info)
	{
		this->name = "GeometryShader";
		this->info = info;

		HRESULT hr = DXDevice::GetDevice().CreateGeometryShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pGeometryShader);

		E2_ASSERT_HR(hr, "GeometryShader CreateGeometryShader failed");
	}

	void GeometryShader::Bind()
	{
		DXDevice::GetContext().GSSetShader(pGeometryShader.Get(), nullptr, 0u);
	}

	void GeometryShader::Unbind()
	{
		DXDevice::GetContext().GSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<GeometryShader> GeometryShader::CreateFromSourceFile(const std::string& filename, const std::string entryPoint, const std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr)) return nullptr;

		return std::make_shared<GeometryShader>(*pBlob.Get(), "Source file: " + filename + "\n" + entryPoint + " " + target);
	}

}
