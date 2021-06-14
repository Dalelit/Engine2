#include "pch.h"
#include "Filewatcher.h"
#include "submodules/imgui/imgui.h"
#include "Logging.h"

namespace Engine2
{
	std::map<const std::string, FileWatcher::FileWatch> FileWatcher::activeWatchers;

	void FileWatcher::Register(const std::string& filename, WatcherCallbackFunc callback)
	{
		FileWatch fw;
		fw.callback = callback;
		fw.reloadCount = 0;
		fw.lastWriteTime = GetLastWriteTime(filename.c_str());

		activeWatchers[filename] = fw;
	}

	void FileWatcher::Unregister(const std::string& filename)
	{
		activeWatchers.erase(filename);
	}

	bool FileWatcher::IsRegistered(const std::string& filename)
	{
		return activeWatchers.find(filename) != activeWatchers.end();
	}

	void FileWatcher::Check()
	{
		for (auto& [k, v] : activeWatchers)
		{
			auto lastWriteTime = GetLastWriteTime(k.c_str());

			if (lastWriteTime.dwLowDateTime != v.lastWriteTime.dwLowDateTime || lastWriteTime.dwHighDateTime != v.lastWriteTime.dwHighDateTime)
			{
				v.lastWriteTime = lastWriteTime;
				v.callback();
				v.reloadCount++;
			}
		}
	}

	void FileWatcher::OnImgui()
	{
		if (ImGui::TreeNode("File watch manager"))
		{
			for (auto& [k, v] : activeWatchers) ImGui::Text("%s, reload count = %i", k.c_str(), v.reloadCount);
			ImGui::TreePop();
		}
	}

	FILETIME FileWatcher::GetLastWriteTime(const char* filename)
	{
		// to do: should error check this.
		WIN32_FILE_ATTRIBUTE_DATA data;
		bool result = GetFileAttributesExA(filename, GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
		if (!result) E2_LOG_WARNING("GetFileAttributesExA failed");
		return data.ftLastWriteTime;
	}

}