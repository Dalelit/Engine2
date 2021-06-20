#pragma once

#include "Layer.h"
#include "Scene.h"

class SceneBuilder : public Engine2::Layer
{
public:
	SceneBuilder();
	~SceneBuilder() = default;

	void OnUpdate(float dt);
	void OnRender();
	void OnApplicationEvent(Engine2::ApplicationEvent& event);
	void OnImgui();

protected:
	Engine2::Scene scene;

	bool onClearIncludeAssets = true;

	void CreateDefaultAssets();
	void CreateEmtpyScene();
	void BuildTestScene();
	void ClearScene(bool forceClearAsset = false);

	void LoadAsset(const std::string& dirname, const std::string& filename);

	// scene directory navigation
	struct FileInfo
	{
		bool isDirectory;
		std::string name;
	};
	std::vector<FileInfo> currentDirectoryContents;
	std::string sceneDirectory;
	std::string currentDirectory;
	std::string selectedFile;

	void ImGuiSceneDirectoy();
	void SetWorkingDirectory(const std::string& dir); // expects the sceneDirectory value to be set
};
