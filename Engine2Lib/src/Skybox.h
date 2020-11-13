#pragma once
#include "Resources.h"
#include "VertexBuffer.h"
#include "Shader.h"

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

		Microsoft::WRL::ComPtr<ID3D11Texture2D1> pTexture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		wrl::ComPtr<ID3D11RasterizerState> pRasterizerState = nullptr;

		std::shared_ptr<VertexShader> vertexShader;
		std::shared_ptr<PixelShader> pixelShader;
		std::shared_ptr<Drawable> vertexBuffer;
	};
}