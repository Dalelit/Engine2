#pragma once
#include "Mesh.h"
#include "VertexBuffer.h"
#include "VertexLayout.h"

namespace Engine2
{
	class MeshComponent
	{
	public:

		MeshComponent();
		~MeshComponent();

		void OnImgui();

		virtual void GenerateMesh();

		std::shared_ptr<Mesh>& GetMesh() { return mesh; }

	protected:
		std::shared_ptr<Mesh> mesh;

		using VertexType = VertexLayout::PositionNormalColor;

		DirectX::XMFLOAT3 origin = { 0.0f, 0.0f, 0.0f };
		float dimensionsWD[2] = { 1.0f, 1.0f };
		uint32_t segmentsWD[2] = { 1, 1 };

		DirectX::XMFLOAT4 color = { 0.8f, 0.3f, 0.1f, 1.0f };

		VertexIndexBufferData<VertexType, uint32_t> vertexData;
		std::shared_ptr<VertexBufferIndex> vertexBuffer;
	};
}