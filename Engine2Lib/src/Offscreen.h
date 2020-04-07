#pragma once
#include "pch.h"
#include "Resources.h"
#include "Shader.h"

namespace Engine2
{
	class Offscreen : public Drawable
	{
	public:
		// Slot is for when binding as a resource to use.
		// Default shaders in here use 0.
		// If needed to be bound for other shaders in a different slot, need to update these shaders

		Offscreen(unsigned int slot = 0); // creates the render target
		Offscreen(unsigned int renderTargetId, unsigned int slot); // reused an existing one

		void Bind(); // binds as a resource
		void Unbind(); // unbinds as a resource
		void SetAsTarget(); // makes this the offscreen target
		void ClearAsTarget(); // makes the back buffer the offscreen target
		void Draw(); // draws the offscreen to the backbuffer
		void Draw(unsigned int toRenderTargetId); // draws the offscreen to another target offscreen

		void OnImgui() { ImGui::Text("Offscreen slot %i, render target id %i", slot, renderTargetId); }

		// Made the shaders public so they can be overwritten.
		// Default to copy. Could replace pixel shader with blur for example.

		std::shared_ptr<VertexShader> pVS = nullptr;
		std::shared_ptr<PixelShader> pPS = nullptr;

	protected:
		unsigned int renderTargetId;
		unsigned int slot;

		std::shared_ptr<Drawable> pDrawable = nullptr;

		void CreateResources();
	};
}