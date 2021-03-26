#include "pch.h"
#include "MeshGenerator.h"
#include <submodules/imgui/imgui.h>

namespace Engine2
{
	namespace MeshGenerators
	{
		bool SimpleMesh::OnImgui()
		{
			bool regenerate = false;

			if (ImGui::DragScalarN("Segments", ImGuiDataType_::ImGuiDataType_U32, segmentsWD, 2, 1u)) regenerate = true;
			if (ImGui::DragFloat2("Dimensions", dimensionsWD, 0.25f)) regenerate = true;
			if (ImGui::DragFloat3("Origin", &origin.x)) regenerate = true;
			if (ImGui::ColorEdit4("Colour", &color.x)) regenerate = true;

			return regenerate;
		}

		void SimpleMesh::GenerateMesh()
		{
			//E2_ASSERT(segmentsWD[0] > 0 && segmentsWD[1] > 0, "Cannot have less than 1 segment");
			if (segmentsWD[0] < 1) segmentsWD[0] = 1;
			if (segmentsWD[1] < 1) segmentsWD[1] = 1;

			size_t vertexCount = (segmentsWD[0] + 1u) * (segmentsWD[1] + 1u);
			size_t indexCount = segmentsWD[0] * segmentsWD[1] * 6u;
			m_vertexData.Resize(vertexCount, indexCount);

			VertexType vertex;
			vertex.position = origin;
			vertex.color = color;
			vertex.normal = { 0.0f, 1.0f, 0.0f };

			VertexType* currentV = m_vertexData.VertexBegin();
			float segWidth = dimensionsWD[0] / (float)segmentsWD[0];
			float segDepth = dimensionsWD[1] / (float)segmentsWD[1];

			for (uint32_t z = 0; z <= segmentsWD[1]; z++)
			{
				vertex.position.x = origin.x;

				for (uint32_t x = 0; x <= segmentsWD[0]; x++)
				{
					*currentV = vertex;

					currentV++;
					vertex.position.x += segWidth;
				}

				vertex.position.z += segDepth;
			}

			uint32_t* currentI = m_vertexData.IndexBegin();
			uint32_t z0 = 0;
			uint32_t z1 = segmentsWD[0] + 1;

			for (uint32_t z = 1; z <= segmentsWD[1]; z++)
			{
				for (uint32_t x = 1; x <= segmentsWD[0]; x++)
				{
					*currentI = z0 + x - 1;
					currentI++;
					*currentI = z1 + x - 1;
					currentI++;
					*currentI = z0 + x;
					currentI++;
					*currentI = z0 + x;
					currentI++;
					*currentI = z1 + x - 1;
					currentI++;
					*currentI = z1 + x;
					currentI++;
				}

				z0 += segmentsWD[0] + 1;
				z1 += segmentsWD[0] + 1;
			}
		}



		bool CheckerMesh::OnImgui()
		{
			bool regenerate = false;

			if (ImGui::DragScalarN("Segments", ImGuiDataType_::ImGuiDataType_U32, segmentsWD, 2, 1u)) regenerate = true;
			if (ImGui::DragFloat2("Dimensions", dimensionsWD, 0.25f)) regenerate = true;
			if (ImGui::DragFloat3("Origin", &origin.x)) regenerate = true;
			if (ImGui::ColorEdit4("Colour1", &color1.x)) regenerate = true;
			if (ImGui::ColorEdit4("Colour2", &color2.x)) regenerate = true;

			return regenerate;
		}

		void CheckerMesh::GenerateMesh()
		{
			if (segmentsWD[0] < 1) segmentsWD[0] = 1;
			if (segmentsWD[1] < 1) segmentsWD[1] = 1;

			uint32_t vertexCount = (segmentsWD[0] + 1) * (segmentsWD[1] + 1);
			uint32_t indexCount = segmentsWD[0] * segmentsWD[1] * 6;
			m_vertexData.Resize(vertexCount * 2, indexCount); // x2 for colors. // to do: could avoid the corner cases but not worth the complexity right now

			VertexType vertex;

			VertexType* currentV = m_vertexData.VertexBegin();
			float segWidth = dimensionsWD[0] / (float)segmentsWD[0];
			float segDepth = dimensionsWD[1] / (float)segmentsWD[1];


			vertex.position = origin;
			vertex.color = color1;
			vertex.normal = { 0.0f, 1.0f, 0.0f };

			for (uint32_t z = 0; z <= segmentsWD[1]; z++)
			{
				vertex.position.x = origin.x;

				for (uint32_t x = 0; x <= segmentsWD[0]; x++)
				{
					*currentV = vertex;

					currentV++;
					vertex.position.x += segWidth;
				}

				vertex.position.z += segDepth;
			}

			vertex.position = origin;
			vertex.color = color2;
			vertex.normal = { 0.0f, 1.0f, 0.0f };

			for (uint32_t z = 0; z <= segmentsWD[1]; z++)
			{
				vertex.position.x = origin.x;

				for (uint32_t x = 0; x <= segmentsWD[0]; x++)
				{
					*currentV = vertex;

					currentV++;
					vertex.position.x += segWidth;
				}

				vertex.position.z += segDepth;
			}

			uint32_t* currentI = m_vertexData.IndexBegin();
			uint32_t vxStride = segmentsWD[0] + 1;

			for (uint32_t z = 1; z <= segmentsWD[1]; z++)
			{
				uint32_t z0 = vxStride * (z - 1);
				uint32_t z1 = z0 + vxStride;

				for (uint32_t x = 1 + (z % 2); x <= segmentsWD[0]; x += 2)
				{
					*currentI = z0 + x - 1;
					currentI++;
					*currentI = z1 + x - 1;
					currentI++;
					*currentI = z0 + x;
					currentI++;
					*currentI = z0 + x;
					currentI++;
					*currentI = z1 + x - 1;
					currentI++;
					*currentI = z1 + x;
					currentI++;
				}
			}

			for (uint32_t z = 1; z <= segmentsWD[1]; z++)
			{
				uint32_t z0 = vertexCount + vxStride * (z - 1);
				uint32_t z1 = z0 + vxStride;

				for (uint32_t x = 1 + ((z + 1) % 2); x <= segmentsWD[0]; x += 2)
				{
					*currentI = z0 + x - 1;
					currentI++;
					*currentI = z1 + x - 1;
					currentI++;
					*currentI = z0 + x;
					currentI++;
					*currentI = z0 + x;
					currentI++;
					*currentI = z1 + x - 1;
					currentI++;
					*currentI = z1 + x;
					currentI++;
				}
			}
		}

	}
}
