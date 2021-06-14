#pragma once

namespace Engine2
{
	class FileWatcher
	{
	public:

		using WatcherCallbackFunc = std::function<void()>;

		static void Register(const std::string& filename, WatcherCallbackFunc callback);
		static void Unregister(const std::string& filename);
		static bool IsRegistered(const std::string& filename);

		static void Check();

		static void OnImgui();

	protected:

		struct FileWatch
		{
			WatcherCallbackFunc callback;
			FILETIME lastWriteTime;
			uint32_t reloadCount;
		};

		static std::map<const std::string, FileWatch> activeWatchers;

		static FILETIME GetLastWriteTime(const char* filename);
	};
}