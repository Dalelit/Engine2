#pragma once
#include "Resources.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Engine2
{
	class Skybox : public Drawable
	{
	public:

		bool Initialise(const std::string& directory);

		void Bind();
		void Draw();
		void Unbind() {}
		void OnImgui();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = (makeActive && status == ""); }

	protected:
		bool active = false;
		unsigned int slot = 0;
		std::string path;
		std::string status = "Uninitialised";

		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		wrl::ComPtr<ID3D11RasterizerState> pRasterizerState = nullptr;

		std::unique_ptr<Texture> texture;
		std::unique_ptr<VertexShader> vertexShader;
		std::unique_ptr<PixelShader> pixelShader;
		std::unique_ptr<Drawable> vertexBuffer;

		bool InitialiseTexture(const std::string& directory);
	};
}