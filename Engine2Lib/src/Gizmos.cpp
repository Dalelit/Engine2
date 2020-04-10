#include "pch.h"
#include "Gizmos.h"
#include "Common.h"

namespace Engine2
{
	#define MAXVERTICIES 20000

	GizmosBuffer::GizmosBuffer() : lineBuffer(MAXVERTICIES), vertexBuffer(MAXVERTICIES)
	{
		VertexShaderLayout vsLayout = { {"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT} };
		std::string vsFileName = Config::directories["ShaderCompiledDir"] + "GizmosVS.cso";
		pVS = VertexShader::CreateFromCompiledFile(vsFileName, vsLayout);

		std::string psFileName = Config::directories["ShaderCompiledDir"] + "GizmosPS.cso";
		pPS = PixelShader::CreateFromCompiledFile(psFileName);
	}

	void GizmosBuffer::NewFrame()
	{
		next = lineBuffer.begin();
		vertexCount = 0;
	}

	void GizmosBuffer::AddLine(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1)
	{
		E2_ASSERT(vertexCount + 2 < MAXVERTICIES, "Exceeded gizmos line buffer");

		*next = p0;
		next++;
		*next = p1;
		next++;

		vertexCount += 2;
	}

	void GizmosBuffer::Draw()
	{
		if (!active) return;

		vertexBuffer.Update(lineBuffer.data(), vertexCount);

		// Note: Assuming the scene has already bound the camera matrix to VS constant buffer 0
		pVS->Bind();
		pPS->Bind();
		vertexBuffer.Bind();
		vertexBuffer.Draw();
	}

	void GizmosBuffer::OnImgui()
	{
		if (ImGui::TreeNode("Gizmos"))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::Text("Vertex count %i", vertexCount);
			ImGui::TreePop();
		}
	}
}
