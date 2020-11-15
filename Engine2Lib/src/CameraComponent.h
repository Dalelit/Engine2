#pragma once
#include "Camera.h"
#include "Offscreen.h"

namespace Engine2
{
	class CameraComponent
	{
	public:

		CameraComponent();

		Camera& GetCamera() { return camera; }

		inline void Clear() { pBuffer->Clear(); }
		inline void SetAsTarget() { pBuffer->SetAsTarget(); }

		inline void ShowSubDisplay() {
			pBuffer->ShowSubDisplayRenderTarget();
			pBuffer->ShowSubDisplayDepthBuffer();
		}

		void OnImgui();

	protected:
		Camera camera;
		std::unique_ptr<OffscreenWithDepthBuffer> pBuffer;
	};
}