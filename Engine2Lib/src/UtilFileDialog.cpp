#include "pch.h"
#include "UtilFileDialog.h"
#include "DXDevice.h"
#include <shobjidl_core.h>

namespace Engine2
{
	namespace Util
	{
		bool FileSelectionDialogue::SelectFilesDialogue(std::vector<std::string>& files)
		{
            bool result = false;
            IFileOpenDialog* pfd;

            // CoCreate the dialog object.
            HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&pfd));

            if (SUCCEEDED(hr))
            {
                DWORD dwOptions;
                // Specify multiselect.
                hr = pfd->GetOptions(&dwOptions);

                if (SUCCEEDED(hr))
                {
                    hr = pfd->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
                }

                if (SUCCEEDED(hr))
                {
                    // Show the Open dialog.
                    hr = pfd->Show(NULL);

                    if (SUCCEEDED(hr))
                    {
                        // Obtain the result of the user interaction.
                        IShellItemArray* psiaResults;
                        hr = pfd->GetResults(&psiaResults);

                        if (SUCCEEDED(hr))
                        {
                            DWORD count;
                            psiaResults->GetCount(&count);

                            IShellItem* pItem;
                            for (auto i = 0u; i < count; i++)
                            {
                                psiaResults->GetItemAt(i, &pItem);
                                if (pItem)
                                {
                                    LPWSTR pszName;
                                    hr = pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszName);
                                    if (SUCCEEDED(hr))
                                    {
                                        files.push_back(Util::ToString(pszName));
                                    }
                                }
                            }

                            result = true;
                            psiaResults->Release();
                        }
                    }
                }
                pfd->Release();
            }
			return result;
		}

        bool FileSelectionDialogue::SelectFolderDialogue(std::string& directoryName)
        {
            // https://docs.microsoft.com/en-us/windows/win32/learnwin32/example--the-open-dialog-box

            bool result = false;

            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            if (SUCCEEDED(hr))
            {
                IFileOpenDialog* pFileOpen;

                // Create the FileOpenDialog object.
                hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                    IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                if (SUCCEEDED(hr))
                {
                    // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions
                    pFileOpen->SetOptions(FOS_PICKFOLDERS);

                    // Show the Open dialog box.
                    hr = pFileOpen->Show(NULL);

                    // Get the file name from the dialog box.
                    if (SUCCEEDED(hr))
                    {
                        IShellItem* pItem;
                        hr = pFileOpen->GetResult(&pItem);
                        if (SUCCEEDED(hr))
                        {
                            PWSTR pszFilePath;
                            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);


                            if (SUCCEEDED(hr))
                            {
                                directoryName = Util::ToString(pszFilePath);
                                CoTaskMemFree(pszFilePath);
                                result = true;
                            }
                            pItem->Release();
                        }
                    }
                    pFileOpen->Release();
                }
                CoUninitialize();
            }
            return result;
        }

		bool FileSelectionDialogue::OpenDialogue(std::string& filename, bool fileMustExist)
		{
			OPENFILENAMEA ofn;
			char szfile[256] = "";

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = DXDevice::Get().GetWindowHandle();
			ofn.lpstrFile = szfile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szfile);
			ofn.lpstrFilter = "All\0*.*"; // "All\0*.*\0Text\0*.TXT\0"
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;
			if (fileMustExist) ofn.Flags |= OFN_FILEMUSTEXIST;

			BOOL result = GetOpenFileNameA(&ofn);

			if (result) filename = szfile;

			return result;
		}
	}
}
