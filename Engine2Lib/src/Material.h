#pragma once
#include "ConstantBuffer2.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureLoader.h"
#include "Transform.h"
#include "UID.h"
#include "Serialiser.h"

namespace Engine2
{
	class Material
	{
	public:
		void Bind() {
			vertexShaderCB.VSBind();
			vertexShader->Bind();
			pixelShaderCB.PSBind();
			pixelShader->Bind();
			if (texture) texture->Bind();
		}

		inline void SetModelData(const TransformMatrix& data) { vertexShaderCB.UpdateBuffer(data.GetTransposed()); }
		inline void BindVSCB() { vertexShaderCB.VSBind(); }

		virtual void OnImgui() = 0;

		//virtual void Serialise(Serialisation::INode& node) = 0; // to do

		virtual std::shared_ptr<Material> Clone() = 0;
		virtual const char* TypeName() = 0;

		const std::string& Name() const { return name; }

		inline UID GetId() const { return id; }
		inline void SetId(UID newId) { id = newId; }

	protected:
		std::string name;
		UID id;

		ConstantBuffer2 vertexShaderCB;
		std::shared_ptr<VertexShader> vertexShader;
		ConstantBuffer2 pixelShaderCB;
		std::shared_ptr<PixelShader> pixelShader;
		std::shared_ptr<Texture> texture;

		void OnImguiCommon()
		{
			if (ImGui::TreeNodeEx("Texture", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE"))
					{
						texture = TextureLoader::Textures[(const char*)payload->Data];
					}
				}

				if (texture)
				{
					texture->OnImgui();
				}
				else
				{
					ImGui::Text("No texture");
					//ImGui::ImageButton(nullptr, { 100, 100 });
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Info"))
			{
				ImGui::Text(TypeName());
				vertexShader->OnImgui();
				pixelShader->OnImgui();
				ImGui::TreePop();
			}
		}
	};
}