#include "pch.h"
#include "MeshComponent.h"

namespace Engine2
{
	MeshComponent::MeshComponent()
	{
		GenerateMesh();
		
		mesh = std::make_shared<Mesh>("Mesh in mesh component"); // to do: something not so lame for the name
		mesh->SetDrawable(vertexBuffer);
	}

	MeshComponent::~MeshComponent()
	{
		//if (verticies) delete verticies;
		//if (indicies) delete indicies;
	}

	void MeshComponent::OnImgui()
	{
		if (ImGui::DragScalarN("Segments", ImGuiDataType_::ImGuiDataType_U32, segmentsWD, 2, 1u)) GenerateMesh();
		if (ImGui::DragFloat2("Dimensions", dimensionsWD, 0.25f)) GenerateMesh();
		if (ImGui::DragFloat3("Origin", &origin.x)) GenerateMesh();
		if (ImGui::ColorEdit4("Colour", &color.x)) GenerateMesh();
	}

	void MeshComponent::GenerateMesh()
	{
		//E2_ASSERT(segmentsWD[0] > 0 && segmentsWD[1] > 0, "Cannot have less than 1 segment");
		if (segmentsWD[0] < 1) segmentsWD[0] = 1;
		if (segmentsWD[1] < 1) segmentsWD[1] = 1;

		size_t vertexCount = (segmentsWD[0] + 1) * (segmentsWD[1] + 1);
		size_t indexCount = segmentsWD[0] * segmentsWD[1] * 6;
		vertexData.Resize(vertexCount, indexCount);

		VertexType vertex;
		vertex.position = origin;
		vertex.color = color;
		vertex.normal = { 0.0f, 1.0f, 0.0f };

		VertexType* currentV = vertexData.VertexBegin();
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

		uint32_t* currentI = vertexData.IndexBegin();
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

		if (vertexBuffer)
		{
			vertexBuffer->Update(vertexData);
		}
		else
		{
			vertexBuffer = std::make_shared<VertexBufferIndex>();
			vertexBuffer->Initialise(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexData, true);
		}
	}
}