#pragma once
#include "pch.h"
#include "VertexBufferDynamic.h"
#include "Shader.h"
#include "Util.h"

namespace Engine2
{
	class Gizmos
	{
	public:
		Gizmos();
		~Gizmos() = default;

		void NewFrame();
		void Draw();
		void OnImgui();

		void DrawLine(DirectX::XMVECTOR p0, DirectX::XMVECTOR p1);
		
		void DrawAxis(DirectX::XMVECTOR pos) { AddLines(pos, AxisWireframe); }

		void DrawSphere(DirectX::XMVECTOR pos) { AddLines(pos, SphereWireframe); }

		inline bool IsActive() { return active; }

	protected:
		bool active = true;
		std::vector<DirectX::XMVECTOR>::iterator next;
		std::vector<DirectX::XMVECTOR> lineBuffer;
		unsigned int vertexCount = 0;

		LineBufferDynamic<DirectX::XMVECTOR> vertexBuffer;
		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;

		void AddLines(DirectX::XMVECTOR pos, std::vector<DirectX::XMVECTOR>& points);

		static std::vector<DirectX::XMVECTOR> AxisWireframe;
		static std::vector<DirectX::XMVECTOR> SphereWireframe;
	};
}