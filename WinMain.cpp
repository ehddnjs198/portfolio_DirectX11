#include "stdafx.h"
#include "Window/Window.h" 

#include "Excute/Execute.h"
#include "Graphic/Graphic.h"
#include "Input/cInput.h"




int APIENTRY WinMain(

	HINSTANCE hInstacne,
	HINSTANCE previnstacne,
	LPSTR ipszCmdParam,
	int nCmdShow
)

{
	//������ ����
	Window::Create(hInstacne, 1280, 720);
	//������ ȭ�麸�̱�
	Window::Show();

	//�̱��� ����.
	Setting::GetInstance().SetWindowHandle(Window::g_handle);
	Setting::GetInstance().SetWindowWidth(
		static_cast<float>(Window::GetWidth()));
	Setting::GetInstance().SetWindowHeight(
		static_cast<float>(Window::GetHeight()));


	Graphic::GetInstance().Initialize();
	Graphic::GetInstance().CreateBackBuffer
	(
		static_cast<UINT>(Setting::GetInstance().GetWindowWidth()),
		static_cast<UINT>(Setting::GetInstance().GetWindowHeight())


	);
	cInput::GetInstance().init(Window::g_instance);



	Execute* execute = new Execute();
	execute->Initialize();
	while (Window::Update())
	{


		cInput::GetInstance().init(Window::g_instance);

		execute->Update();
		execute->Render();

	}

	

	SAFE_DELETE(execute);
	Window::Destroy();
	return 0;
}
