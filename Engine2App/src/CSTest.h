#pragma once

#include "Layer.h"
#include "Scene.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "TextureGizmo.h"

class CSTest : public Engine2::Layer
{
public:
	CSTest();
	~CSTest() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnImgui();

protected:
	std::shared_ptr<Engine2::ComputeShader> pComputeShader;

	std::unique_ptr<Engine2::Texture> tex1;
	std::unique_ptr<Engine2::Texture> tex2;

	Engine2::ComputeShaderSRV texsrv1;
	Engine2::ComputeShaderSRV texsrv2;

	Engine2::ComputeShaderUAV texuav1;
	Engine2::ComputeShaderUAV texuav2;

	Engine2::TextureGizmo tex1Gizmo;
	Engine2::TextureGizmo tex2Gizmo;

	UINT state = 0;
};
#pragma once
