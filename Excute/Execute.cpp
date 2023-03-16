#include "stdafx.h"
#include "Excute/Execute.h"
#include "Graphic/Graphic.h"

#include "Inspector/Inspector.h"

#include "Input/cInput.h"

#include"singletonBox.h"
#include <stdlib.h>
#include "FileSave.h"

Execute::Execute()
{

}

Execute::~Execute()	
{

}




void Execute::Initialize()
{
	_inspector = new Inspector();
	_inspector->Initialize();

	_fileSave = new FileSave();
}

void Execute::Update()
{
	_inspector->Update();

	if (BOX->isOpen)
	{
		Open();

	}
	if (BOX->fileSave)
	{
		_fileSave->FileWrite();
	}

}

void Execute::Render()
{
	Graphic::GetInstance().Begin();
	{
		_inspector->Render();


		
	}
	Graphic::GetInstance().End();
}


void Execute::Open()
{
	BOX->isOpen = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					//
					BOX->isLoad = true;
					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
		BOX->isMonsterOpen();
	}
}