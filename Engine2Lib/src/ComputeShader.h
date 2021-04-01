#pragma once
#include "Shader.h"
#include "ComputeShaderBuffer.h"

namespace Engine2
{
	class ComputeShader : public Shader
	{
	public:
		ComputeShader(ID3DBlob& shaderBlob, std::string info);

		void Dispatch();

		void Bind();
		void Unbind();

		void SetThreadGroupCount(UINT x, UINT y, UINT z) { threadGroupCount[0] = x; threadGroupCount[1] = y; threadGroupCount[2] = z; }

		static std::shared_ptr<ComputeShader> CreateFromSourceFile(const std::string& filename, const std::string entryPoint = "main", const std::string target = "cs_5_0");

	protected:
		ComputeShader() = default;
		wrl::ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;

		UINT threadGroupCount[3] = { 1,1,1 };
	};

	class ComputeShaderFile : public ComputeShader
	{
		ComputeShaderFile(const std::string& filename, const std::string entryPoint = "main", const std::string target = "ps_5_0");

		void Bind() { if (autoReload) Reload(); ComputeShader::Bind(); }
		void Reload(); // only reloads if the file has changed
		inline bool IsValid() { return (pComputeShader); }
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
}