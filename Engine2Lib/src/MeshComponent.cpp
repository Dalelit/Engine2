#include "pch.h"
#include "MeshComponent.h"

namespace Engine2
{
	MeshComponent::MeshComponent()
	{
		generator = std::make_shared<MeshGenerators::CheckerMesh>(vertexData);
		generatorNameActive = "CheckerMesh";

		GenerateMesh();
		
		mesh = std::make_shared<Mesh>("Mesh in mesh component"); // to do: something not so lame for the name
		mesh->SetVertexBuffer(vertexBuffer);
	}


	void MeshComponent::OnImgui()
	{
		if (ImGui::BeginCombo("Generator", generatorNameActive.c_str()))
		{
			ImguiGeneratorComboHelper<MeshGenerators::SimpleMesh>("SimpleMesh");
			ImguiGeneratorComboHelper<MeshGenerators::CheckerMesh>("CheckerMesh");
			ImGui::EndCombo();
		}

		if (generator->OnImgui())
			GenerateMesh();
	}

	void MeshComponent::GenerateMesh()
	{
		generator->GenerateMesh();

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