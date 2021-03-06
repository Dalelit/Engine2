#pragma once
#include "VertexBuffer.h"
#include "Shader.h"

namespace Engine2
{
	class TextureGizmo
	{
	public:
		TextureGizmo(UINT textureSlot);

		void OnImgui();

		void RenderTexture();

		UINT GetTextureSlot() { return slot; }

	private:
		UINT slot = 2;
		bool active = true;

		std::shared_ptr<VertexBuffer> pVB;
		std::shared_ptr<VertexShader> pVS;
		std::shared_ptr<PixelShader>  pPS;

		float leftTop[2] = { 0.5f, -0.5f };
		float widthHeight[2] = { 0.25f, 0.25f };

		void InitialiseShaders();

		inline void CreateVertexBuffer() { CreateVertexBuffer(leftTop[0], leftTop[1], leftTop[0] + widthHeight[0], leftTop[1] - widthHeight[1]); };
		void CreateVertexBuffer(float left, float top, float right, float bottom);
	};
}
