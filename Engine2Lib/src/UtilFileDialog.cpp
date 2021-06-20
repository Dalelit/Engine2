#include "pch.h"
#include "UtilFileDialog.h"
#include "DXDevice.h"
#include <shobjidl_core.h>

// https://docs.microsoft.com/en-us/windows/win32/learnwin32/example--the-open-dialog-box
// https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions

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
            bool result = false;
            HRESULT hr;

            //hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            //if (SUCCEEDED(hr))
            {
                IFileOpenDialog* pFileOpen;

                // Create the FileOpenDialog object.
                hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                    IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                if (SUCCEEDED(hr))
                {
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
                //CoUninitialize();
            }
            return result;
        }

		bool FileSelectionDialogue::OpenDialogue(std::string& filename, bool fileMustExist)
		{
            bool result = false;
            HRESULT hr;
            
            //hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            //if (SUCCEEDED(hr))
            {
                IFileOpenDialog* pFileOpen;

                // Create the FileOpenDialog object.
                hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                    IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                if (SUCCEEDED(hr))
                {
                    if (fileMustExist)
                        pFileOpen->SetOptions(FOS_FILEMUSTEXIST);
                    else
                        pFileOpen->SetOptions(FOS_PATHMUSTEXIST);

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
                                filename = Util::ToString(pszFilePath);
                                CoTaskMemFree(pszFilePath);
                                result = true;
                            }
                            pItem->Release();
                        }
                    }
                    pFileOpen->Release();
                }
                //CoUninitialize();
            }
            return result;
        }
	}
}
