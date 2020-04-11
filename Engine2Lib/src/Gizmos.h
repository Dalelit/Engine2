#pragma once
#include "pch.h"
#include "VertexBufferDynamic.h"
#include "Shader.h"
#include "Util.h"

namespace Engine2
{
	class GizmosBuffer
	{
	public:
		GizmosBuffer();
		~GizmosBuffer() = default;

		void NewFrame();
		void Draw();
		void OnImgui();

		void DrawLine(DirectX::XMVECTOR p0, DirectX::XMVECTOR p1);
		
		void DrawAxis(DirectX::XMVECTOR p0);

		void DrawSphere(DirectX::XMVECTOR p0);

		inline bool IsActive() { return active; }

	protected:
		bool active = true;
		std::vector<DirectX::XMVECTOR>::iterator next;
		std::vector<DirectX::XMVECTOR> lineBuffer;
		unsigned int vertexCount = 0;

		LineBufferDynamic<DirectX::XMVECTOR> vertexBuffer;
		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;

		static std::vector<DirectX::XMVECTOR> SphereWireframe;
	};
}