#pragma once
#include "Mesh.h"
#include "VertexBuffer.h"
#include "VertexLayout.h"
#include "MeshGenerator.h"

namespace Engine2
{
	class MeshComponent
	{
	public:

		MeshComponent();

		void OnImgui();

		void GenerateMesh();

		std::shared_ptr<Mesh>& GetMesh() { return mesh; }

	protected:
		std::shared_ptr<Mesh> mesh;
		using VertexType = VertexLayout::PositionNormalColor;
		VertexIndexBufferData<VertexType, uint32_t> vertexData;
		std::shared_ptr<VertexBufferIndex> vertexBuffer;

		std::shared_ptr<MeshGenerator> generator;
		std::string generatorNameActive;

		template <typename T>
		void ImguiGeneratorComboHelper(const char* name)
		{
			if (ImGui::Selectable(name, (generatorNameActive == name)))
			{
				generator = std::make_shared<T>(vertexData);
				generatorNameActive = name;
				GenerateMesh();
			}
		}

	};
}