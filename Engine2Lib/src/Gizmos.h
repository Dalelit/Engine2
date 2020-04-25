#pragma once
#include "pch.h"
#include "VertexBufferInstanced.h"
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
		void Render();
		void OnImgui();

		void DrawAxis(DirectX::XMMATRIX instance);

		void DrawSphere(DirectX::XMMATRIX instance);

		inline bool IsActive() { return active; }

	protected:
		bool active = true;

		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;

		// axis
		std::vector<DirectX::XMMATRIX> axisInstances;
		unsigned int axisInstanceCount = 0;
		VertexBufferIndexInstanced<DirectX::XMFLOAT3, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST> axisVBuffer;
		ID3D11Buffer* axisPtrInstancesBuffer;
		static std::vector<DirectX::XMFLOAT3> AxisVerticies;
		static std::vector<unsigned int> AxisIndicies;

		// sphere
		std::vector<DirectX::XMMATRIX> sphereInstances;
		unsigned int sphereInstanceCount = 0;
		VertexBufferIndexInstanced<DirectX::XMFLOAT3, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST> sphereVBuffer;
		ID3D11Buffer* spherePtrInstancesBuffer;
		static std::vector<DirectX::XMFLOAT3> SphereVerticies;
		static std::vector<unsigned int> SphereIndicies;
	};
}