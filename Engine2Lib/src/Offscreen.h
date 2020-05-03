#pragma once
#include "pch.h"
#include "Resources.h"
#include "Shader.h"

namespace Engine2
{
	class Offscreen
	{
	public:
		// Slot is for when binding as a resource to use.
		// Default shaders in here use 0.
		// If needed to be bound for other shaders in a different slot, need to update these shaders

		Offscreen(unsigned int slot = 0);
		virtual ~Offscreen() = default;

		// use as a shader resource
		void Bind();   // binds as a resource
		void Unbind(); // unbinds as a resource
		
		// use as a target
		virtual void SetAsTarget();    // makes this the offscreen target
		
		// draw the results
		void DrawToBackBuffer(); // draws the offscreen to the back buffer
		virtual void Clear();    // clears it for the frame

		// manage the resources
		virtual void Release();  // releases the resources, e.g when resizing
		virtual void Configure() // configures the resources. Override the methods to replace behaviour
		{
			InitialiseBuffer();
			InitialiseShaderResources();
		}
		virtual void Reconfigure() // call when there has been a resize
		{
			Release();
			Configure();
		}

		inline UINT GetWidth()  { return width; }
		inline UINT GetHeight() { return height; }
		inline wrl::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return pTargetView; }

		void OnImgui();

	protected:
		std::shared_ptr<Drawable> pDrawable = nullptr;

		// render target
		wrl::ComPtr<ID3D11Texture2D> pBuffer = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> pTargetView = nullptr;
		// set when the buffer is created so it can be used by other resources
		UINT width;
		UINT height;
		DXGI_FORMAT format;

		// shader resources
		unsigned int slot;
		wrl::ComPtr<ID3D11ShaderResourceView> pResourceView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> pSamplerState = nullptr;

		// Shaders.
		// Default to copy. Could replace pixel shader with blur for example.
		std::shared_ptr<VertexShader> pVS = nullptr;
		std::shared_ptr<PixelShader> pPS = nullptr;

		virtual void InitialiseDrawResources();
		
		virtual void InitialiseBuffer();
		virtual void InitialiseShaderResources();
	};

	class OffscreenWithDepthBuffer : public Offscreen
	{
	public:

		OffscreenWithDepthBuffer(unsigned int slot = 0) : Offscreen(slot)
		{
			InitialiseDepthBuffer();
		}

		void SetAsTarget();
		void Clear();

		void Release();
		void Configure()
		{
			InitialiseBuffer();
			InitialiseDepthBuffer();
			InitialiseShaderResources();
		}

	protected:
		// depth buffer
		wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11Texture2D> pDepthTexture = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		UINT stencilRef = 0;

		virtual void InitialiseDepthBuffer();
	};
}