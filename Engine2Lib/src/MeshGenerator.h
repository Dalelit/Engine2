#pragma once
#include "VertexBufferData.h"
#include "VertexLayout.h"

namespace Engine2
{
	class MeshGenerator
	{
	public:

		using VertexType = VertexLayout::PositionNormalColor;

		MeshGenerator(VertexIndexBufferData<VertexType>& vertexData) : m_vertexData(vertexData) {}

		virtual bool OnImgui() = 0; // return true if the mesh needs to be regenerated
		virtual void GenerateMesh() = 0;

	protected:
		VertexIndexBufferData<VertexType>& m_vertexData;
	};

	namespace MeshGenerators
	{
		// create a rectangle mesh
		class SimpleMesh : public MeshGenerator
		{
		public:
			SimpleMesh(VertexIndexBufferData<VertexType>& vertexData) : MeshGenerator(vertexData) {}

			bool OnImgui();
			void GenerateMesh();

		protected:
			DirectX::XMFLOAT3 origin = { 0.0f, 0.0f, 0.0f };
			float dimensionsWD[2] = { 1.0f, 1.0f };
			uint32_t segmentsWD[2] = { 1, 1 };
			DirectX::XMFLOAT4 color = { 0.8f, 0.3f, 0.1f, 1.0f };
		};

		// create a rectangle checkered mesh
		class CheckerMesh : public MeshGenerator
		{
		public:
			CheckerMesh(VertexIndexBufferData<VertexType>& vertexData) : MeshGenerator(vertexData) {}

			bool OnImgui();
			void GenerateMesh();

		protected:
			DirectX::XMFLOAT3 origin = { 0.0f, 0.0f, 0.0f };
			float dimensionsWD[2] = { 1.0f, 1.0f };
			uint32_t segmentsWD[2] = { 1, 1 };
			DirectX::XMFLOAT4 color1 = { 0.8f, 0.3f, 0.1f, 1.0f };
			DirectX::XMFLOAT4 color2 = { 0.1f, 0.3f, 0.8f, 1.0f };
		};
	}
}