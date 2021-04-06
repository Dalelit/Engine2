#include "pch.h"
#include "ComputeShader.h"

namespace Engine2
{
	ComputeShader::ComputeShader(ID3DBlob& shaderBlob, std::string info)
	{
		this->name = "ComputeShader";
		this->info = info;

		HRESULT hr = DXDevice::GetDevice().CreateComputeShader(
			shaderBlob.GetBufferPointer(),
			shaderBlob.GetBufferSize(),
			nullptr,
			&pComputeShader);

		E2_ASSERT_HR(hr, "ComputeShader CreateComputeShader failed");
	}

	void ComputeShader::Dispatch()
	{
		DXDevice::GetContext().Dispatch(threadGroupCount[0], threadGroupCount[1], threadGroupCount[2]);
	}

	void ComputeShader::Bind()
	{
		DXDevice::GetContext().CSSetShader(pComputeShader.Get(), nullptr, 0u);
	}

	void ComputeShader::Unbind()
	{
		DXDevice::GetContext().CSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<ComputeShader> ComputeShader::CreateFromSourceFile(const std::string& filename, const std::string entryPoint, const std::string target)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompileFromFile(Util::ToWString(filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			std::string errorMsg = (char*)pErrBlob->GetBufferPointer();
			Logging::LogError("Compute shader failed.");
			Logging::LogError(errorMsg);
			return nullptr;
		}

		return std::make_shared<ComputeShader>(*pBlob.Get(), "Source file: " + filename + "\n" + entryPoint + " " + target);
	}

	ComputeShaderFile::ComputeShaderFile(const std::string& filename, const std::string entryPoint, const std::string target) :
		filename(filename), entryPoint(entryPoint), target(target), fileWatcher(filename)
	{
		this->name = "ComputeShaderFile";
		this->info = "Source file: " + filename + "\n" + entryPoint + " " + target;

		Load();
	}

	void ComputeShaderFile::Reload()
	{
		if (fileWatcher.Check())
		{
			pComputeShader.Reset();
			Load();
		}
	}

	void ComputeShaderFile::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text(info.c_str());
			ImGui::Text(status.c_str());
			ImGui::Checkbox("Auto update", &autoReload);
			ImGui::TreePop();
		}
	}

	void ComputeShaderFile::Load()
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

		hr = DXDevice::GetDevice().CreateComputeShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pComputeShader);

		if (FAILED(hr))
		{
			status = "Failed to CreateComputeShader";
			return;
		}

		status = "Loaded";
	}
}