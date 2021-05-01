#pragma once
#include "pch.h"
#include "VertexBuffer.h"
#include "Shader.h"

namespace Engine2
{
	class Offscreen
	{
	public:
		// Slot is for when binding as a resource to use.
		// Default shaders in here use 0.
		// If needed to be bound for other shaders in a different slot, need to update these shaders

		struct Descriptor
		{
			UINT slot = 0;
			bool hasRenderTarget = true;
			bool hasDepthBuffer = true;
			bool unorderedAccess = false;
			int  DXGIFormat = -1; // -1 will use back buffer format
			int  height = -1; // -1 will use device height and width
			int  width = -1;
		};

		Offscreen(unsigned int slot = 0) : Offscreen(true, true, slot) {}
		Offscreen(bool hasRenderTarget, bool hasDepthBuffer, unsigned int slot = 0);
		Offscreen(Descriptor desc) { Initialise(desc); }

		void Initialise(Descriptor desc);

		// use as a shader resource
		inline void Bind() { BindBuffer(descriptor.slot); }   // binds as a resource. defaults to binding the render target
		void BindBuffer(unsigned int textureSlot);
		void BindDepthBuffer(unsigned int textureSlot);
		void Unbind(); // unbinds as a resource
		
		// use as a target
		void SetAsTarget();    // makes this the offscreen target
		
		// draw the results
		void DrawToBackBuffer(); // draws the offscreen to the back buffer
		void Clear();    // clears it for the frame

		// manage the resources
		void Release();     // releases the resources, e.g when resizing
		void Configure();   // configures the resources. Override the methods to replace behaviour
		void Reconfigure(); // call when there has been a resize

		inline UINT GetWidth()  { return width; }
		inline UINT GetHeight() { return height; }
		inline wrl::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return pTargetView; }

		inline bool HasRenderTarget() { return descriptor.hasRenderTarget; }
		inline bool HasDepthBuffer()  { return descriptor.hasDepthBuffer; }

		inline void SetPixelShader(std::shared_ptr<PixelShader> pNewPixelShader) { pPS = pNewPixelShader; }

		void OnImgui();
		void ShowSubDisplay();

		inline Microsoft::WRL::ComPtr<ID3D11Texture2D> GetBuffer() { return pBuffer; }
		inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() { return pBufferResourceView; }
		// note: will add the depth buffer Get's if ever needed in the future

		static std::map<std::string, std::shared_ptr<PixelShader>> pixelShaders; // Public so can add to this. Populated with AddSampleFilters on constructuion.

	protected:
		Descriptor descriptor;

		// render target
		wrl::ComPtr<ID3D11Texture2D> pBuffer = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> pTargetView = nullptr;
		wrl::ComPtr<ID3D11ShaderResourceView> pBufferResourceView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> pBufferSamplerState = nullptr;

		// depth buffer
		wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11Texture2D> pDepthBuffer = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		UINT stencilRef = 0;
		wrl::ComPtr<ID3D11ShaderResourceView> pDepthBufferResourceView;
		wrl::ComPtr<ID3D11SamplerState> pDepthBufferSamplerState = nullptr;

		// set when the buffer is created so it can be used by other resources
		UINT width;
		UINT height;

		// Shaders.
		// Default to copy. Could replace pixel shader with blur for example.
		std::shared_ptr<PixelShader> pPS = nullptr;
		std::string pixelShaderName;

		void InitialiseBuffer();
		void InitialiseDepthBuffer();
		void ReleaseBuffer();
		void ReleaseDepthBuffer();

		static std::shared_ptr<VertexBuffer> CreateVertexBuffer(float left, float top, float right, float bottom);

		// subwindow display of render target
		struct {
			bool show = true;
			float leftTop[2] = { 0.5f, -0.5f };
			float size = 0.5f;
			std::shared_ptr<VertexBuffer> pVB;
		} subDisplay;
		void InitialiseSubDisplayVB();

		// subwindow display of depth buffer
		struct {
			bool show = true;
			float leftTop[2] = { 0.5f, 0.0f };
			float size = 0.5f;
			bool displayRaw = false;
			std::shared_ptr<VertexBuffer> pVB;
		} subDisplayDepthBuffer;
		void InitialiseSubDisplayDepthBufferVB();

		// Common resources created once and reused

		static struct OffscreenCommonResources {

			bool initialised = false;

			struct {
				std::shared_ptr<VertexBuffer> pVB;
				std::shared_ptr<VertexShader> pVS;
			} ForDrawToBackBuffer;

			struct {
				std::shared_ptr<VertexShader> pVS;
				std::shared_ptr<PixelShader>  pPSBuffer;
				std::shared_ptr<PixelShader>  pPSDepthBuffer;
				std::shared_ptr<PixelShader>  pPSDepthBufferRaw;
			} ForDrawToSubDisplay;

		} Common;

		static void InitialiseCommon();
		static void AddSampleFilters();
	};
}