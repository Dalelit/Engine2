#pragma once
#include "pch.h"
#include "VertexBufferInstanced.h"
#include "Shader.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	class GizmoRender
	{
		using GizmoVBType = VertexBufferIndexInstanced<DirectX::XMFLOAT3, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>;
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
		wrl::ComPtr<ID3D11Buffer> instanceBuffer;
		using InstanceInfoType = DirectX::XMMATRIX;

		void CreateVertexBuffers();

		void Draw();

		// axis
		std::vector<InstanceInfoType> axisInstances;
		std::unique_ptr<GizmoVBType> axisVBuffer;

		// sphere
		std::vector<InstanceInfoType> sphereInstances;
		std::unique_ptr<GizmoVBType> sphereVBuffer;

		// cube
		std::vector<InstanceInfoType> cubeInstances;
		std::unique_ptr<GizmoVBType> cubeVBuffer;

		// camera
		std::vector<InstanceInfoType> cameraInstances;
		std::unique_ptr<GizmoVBType> cameraVBuffer;
	};
}