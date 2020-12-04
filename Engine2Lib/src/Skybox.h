#pragma once
#include "Resources.h"
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

	class Skybox : public Drawable
	{
	public:

		bool Initialise(const std::string& directory, const std::string& fileType = "jpg");
		bool Initialise(std::vector<std::string>& filenames);

		void Bind();
		void Draw();
		void Unbind() {}
		void OnImgui();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = (makeActive && status == ""); }

	protected:
		bool active = false;
		unsigned int slot = 0;
		std::vector<std::string> filelist;
		std::string status = "Uninitialised";

		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		wrl::ComPtr<ID3D11RasterizerState> pRasterizerState = nullptr;

		std::unique_ptr<Texture> texture;
		std::unique_ptr<VertexShader> vertexShader;
		std::unique_ptr<PixelShader> pixelShader;
		std::unique_ptr<VertexBufferIndex> vertexBuffer;

		SkyboxFileSelector fileSelection;

		bool InitialiseTexture(std::vector<std::string>& filenames);
	};
}