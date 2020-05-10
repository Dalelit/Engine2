#pragma once
#include "Offscreen.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	// to use:
	//   outliner.SetForMask();
	//   pSelectedModel->RenderDrawablesOnly();
	//   outliner.SetForOutline();
	//   pSelectedModel->RenderDrawablesOnly();
	//   outliner.DrawToBackBuffer();
	// to do: put this as a method in here?


	class OffscreenOutliner
	{
	public:
		OffscreenOutliner();
		void Clear();
		void DrawToBackBuffer();
		void SetForWriteMask();
		void SetForOutline();

		void Configure();
		void Reset();
		void Reconfigure() { offscreen.Reconfigure(); Reset(); Configure(); }

		void OnImgui();

	protected:
		Offscreen offscreen;

		wrl::ComPtr<ID3D11DepthStencilState> pDSSWriteMask;
		wrl::ComPtr<ID3D11DepthStencilState> pDSSUseMask;
		wrl::ComPtr<ID3D11DepthStencilView> pDSVStencil;
		wrl::ComPtr<ID3D11Texture2D> pDTStencil;

		float outlineScale = 1.1f;
		VSConstantBuffer<DirectX::XMVECTOR> vsOutlineCB;
		std::shared_ptr<Engine2::VertexShader> pVSOutline;

		DirectX::XMVECTOR outlineColor = { 0.8f, 0.8f, 0.2f, 0.75f };
		PSConstantBuffer<DirectX::XMVECTOR> psOutlineCB;
		std::shared_ptr<Engine2::PixelShader> pPSOutline;

		void Initialise();
	};
}