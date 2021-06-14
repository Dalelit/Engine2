#pragma once

namespace Engine2
{
	namespace Util
	{
		class FileSelectionDialogue
		{
		public:
			inline static bool LoadDialogue(std::string& filename) { return OpenDialogue(filename, true); }
			inline static bool SaveDialogue(std::string& filename) { return OpenDialogue(filename, false); }

			static bool SelectFilesDialogue(std::vector<std::string>& files);

			static bool SelectFolderDialogue(std::string& directoryName);

		protected:
			static bool OpenDialogue(std::string& filename, bool fileMustExist);
		};
	}
}
