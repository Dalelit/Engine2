#pragma once
#include "pch.h"
#include "VertexBufferInstanced.h"
#include "Shader.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	class GizmoRender
	{
	public:

		GizmoRender(size_t maxGizmos = 50);
		~GizmoRender() = default;

		void NewFrame();
		void Render();

		void DrawAxis(DirectX::XMMATRIX instance);
		void DrawSphere(DirectX::XMMATRIX instance);
		void DrawCube(DirectX::XMMATRIX instance);
		void DrawCamera(DirectX::XMMATRIX instance);

	protected:
		size_t maxGizmos;
		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;
		PSConstantBuffer<DirectX::XMVECTOR> psCB;
		DirectX::XMVECTOR visibleColor = { 0.2f, 0.8f, 0.2f, 1.0f };
		DirectX::XMVECTOR hiddenColor = { 0.05f, 0.2f, 0.05f, 1.0f };
		wrl::ComPtr<ID3D11DepthStencilState> pBackDrawDSS;
		using InstanceInfoType = DirectX::XMMATRIX;

		void CreateVertexBuffers();

		void Draw();

		// axis
		std::vector<InstanceInfoType> axisInstances;
		VertexBufferIndexInstanced axisVBuffer;

		// sphere
		std::vector<InstanceInfoType> sphereInstances;
		VertexBufferIndexInstanced sphereVBuffer;

		// cube
		std::vector<InstanceInfoType> cubeInstances;
		VertexBufferIndexInstanced cubeVBuffer;

		// camera
		std::vector<InstanceInfoType> cameraInstances;
		VertexBufferIndexInstanced cameraVBuffer;
	};
}