#pragma once
#include "pch.h"
#include "VertexBufferDynamic.h"
#include "Shader.h"

namespace Engine2
{
	class GizmosBuffer
	{
	public:
		GizmosBuffer();
		~GizmosBuffer() = default;

		void NewFrame();
		void AddLine(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1);
		void Draw();
		void OnImgui();

		inline bool IsActive() { return active; }

	protected:
		bool active = true;
		std::vector<DirectX::XMFLOAT3>::iterator next;
		std::vector<DirectX::XMFLOAT3> lineBuffer;
		unsigned int vertexCount = 0;

		LineBufferDynamic<DirectX::XMFLOAT3> vertexBuffer;
		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;
	};
}