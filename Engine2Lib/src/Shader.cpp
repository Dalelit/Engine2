#include "pch.h"
#include "Common.h"
#include "Shader.h"
#include "Filewatcher.h"

namespace Engine2
{
	void Shader::OnImgui()
	{
		if (ImGui::TreeNode(mName.c_str()))
		{
			ImGui::Text("Filename %s", mFilename.c_str());
			ImGui::Text("Entry point %s", mEntryPoint.c_str());
			ImGui::Text("Target %s", mTarget.c_str());
			if (!mFilename.empty())
			{
				if (ImGui::Button("Reload")) Reload();
				ImGui::SameLine();

				if (FileWatcher::IsRegistered(mFilename))
				{
					if (ImGui::Button("Stop auto reload")) FileWatcher::Unregister(mFilename);
				}
				else
				{
					if (ImGui::Button("Auto reload")) FileWatcher::Register(mFilename, [this]() { Reload(); });
				}
			}
			ImGui::TreePop();
		}
	}

	void Shader::Reload()
	{
		if (!mFilename.empty())
		{
			if (Util::StringEndsWith(mFilename, ".cso"))
			{
				LoadFromFile();
			}
			else // assume ends in .hlsl
			{
				CompileFromFile();
			}
		}
	}

	void Shader::LoadFromFile(const std::string& filename)
	{
		mFilename = filename;
		mEntryPoint = "";
		mTarget = "";

		LoadFromFile();
	}

	void Shader::LoadFromFile()
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		HRESULT hr = D3DReadFileToBlob(Util::ToWString(mFilename).c_str(), &pBlob);

		if (FAILED(hr))
		{
			Logging::LogError("Failed to load file: " + mFilename);
		}
		else
		{
			Initialise(pBlob);
		}
	}

	void Shader::CompileFromFile(const std::string& filename, const std::string& entryPoint, const std::string& target)
	{
		mFilename = filename;
		mEntryPoint = entryPoint;
		mTarget = target;

		CompileFromFile();
	}

	void Shader::CompileFromFile()
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;
		HRESULT hr = D3DCompileFromFile(Util::ToWString(mFilename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, mEntryPoint.c_str(), mTarget.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			Logging::LogError("Failed to compile file: " + mFilename);
			if (pErrBlob) Logging::LogError((char*)pErrBlob->GetBufferPointer());
		}
		else
		{
			Initialise(pBlob);
		}
	}

	void Shader::CompileFromSource(const std::string& source, const std::string& entryPoint, const std::string& target)
	{
		mFilename = "";
		mEntryPoint = entryPoint;
		mTarget = target;

		wrl::ComPtr<ID3DBlob> pBlob;
		wrl::ComPtr<ID3DBlob> pErrBlob;

		HRESULT hr = D3DCompile(source.data(), source.length(), NULL, NULL, NULL, mEntryPoint.c_str(), mTarget.c_str(), 0, 0, &pBlob, &pErrBlob);

		if (FAILED(hr))
		{
			Logging::LogError("Failed to compile source string.");
			if (pErrBlob) Logging::LogError((char*)pErrBlob->GetBufferPointer());
		}
		else
		{
			Initialise(pBlob);
		}
	}

	void VertexShader::Initialise(const wrl::ComPtr<ID3DBlob>& pBlob)
	{
		E2_ASSERT(pBlob, "Blob is null");
		mName = "VertexShader";

		HRESULT hr;

		hr = DXDevice::GetDevice().CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVertexShader);

		//E2_ASSERT_HR(hr, "VertexShader CreateVertexShader failed");
		if (FAILED(hr))
		{
			Logging::LogError("VertexShader CreateVertexShader failed");
			return;
		}

		hr = DXDevice::GetDevice().CreateInputLayout(
			mLayout.data(),
			(UINT)mLayout.size(),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout);

		//E2_ASSERT_HR(hr, "VertexShader CreateInputLayout failed");
		if (FAILED(hr))
		{
			Logging::LogError("VertexShader CreateInputLayout failed");
			return;
		}
	}

	void PixelShader::Initialise(const wrl::ComPtr<ID3DBlob>& pBlob)
	{
		E2_ASSERT(pBlob, "Blob is null");
		mName = "PixelShader";

		HRESULT hr = DXDevice::GetDevice().CreatePixelShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pPixelShader);

		//E2_ASSERT_HR(hr, "PixelShader CreatePixelShader failed");
		if (FAILED(hr))
		{
			Logging::LogError("PixelShader CreatePixelShader failed");
			return;
		}
	}

	void GeometryShader::Initialise(const wrl::ComPtr<ID3DBlob>& pBlob)
	{
		E2_ASSERT(pBlob, "Blob is null");
		mName = "GeometryShader";

		HRESULT hr = DXDevice::GetDevice().CreateGeometryShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pGeometryShader);

		//E2_ASSERT_HR(hr, "GeometryShader CreateGeometryShader failed");
		if (FAILED(hr))
		{
			Logging::LogError("GeometryShader CreateGeometryShader failed");
			return;
		}
	}

	void ComputeShader::Initialise(const wrl::ComPtr<ID3DBlob>& pBlob)
	{
		E2_ASSERT(pBlob, "Blob is null");
		mName = "ComputeShader";

		HRESULT hr = DXDevice::GetDevice().CreateComputeShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pComputeShader);

		//E2_ASSERT_HR(hr, "ComputeShader CreateComputeShader failed");
		if (FAILED(hr))
		{
			Logging::LogError("ComputeShader CreateComputeShader failed");
			return;
		}
	}
}
