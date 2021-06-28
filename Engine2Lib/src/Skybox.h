#pragma once
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Engine2
{
	class SkyboxFileSelector
	{
	public:
		void OnImgui();

		std::vector<std::string>& GetFilenames() { return filenames; }

	protected:
		std::vector<std::string> filenames;
	};

	class Skybox
	{
	public:

		bool Initialise(const std::string& directory, const std::string& fileType = "jpg");
		bool Initialise(const std::vector<std::string>& filenames = {});

		void Bind();
		void Draw();
		inline void BindAndDraw() { Bind(); Draw(); }
		virtual void OnImgui();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = makeActive; }

		void SetPixelShader(const std::string& filename) { pixelShader.CompileFromFile(filename); }

	protected:
		bool active = false;
		unsigned int slot = 0;
		std::vector<std::string> filelist;

		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		wrl::ComPtr<ID3D11RasterizerState> pRasterizerState = nullptr;

		std::unique_ptr<Texture> texture;
		VertexShader vertexShader;
		PixelShader pixelShader;
		std::unique_ptr<VertexBufferIndex> vertexBuffer;

		SkyboxFileSelector fileSelection;

		bool InitialiseTexture(const std::vector<std::string>& filenames);
	};
}