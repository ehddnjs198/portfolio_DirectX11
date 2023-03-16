#pragma once

#include "stdafx.h"

//#include "imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
	/*
	���� �����̽��� ����ϴ� ������ �⺻������ C++ ����ϴµ�
	��ü�����̶�� �θ���., ��ü�����̶� �θ��°��� �����ϰ� ���ϸ�
	���������ϰ� �Ѵٴ°�, ���뼺�� ���̴°��̴�.
	���� ������ ������ �� �ȵȴ�. �׷��� ������.
	���� C++������ ��������, �Լ��� ����°��� namespacew�� �����ִ°� ���ϴ�.
	*/
{
	static HINSTANCE g_instance;
	static HWND g_handle;
	/*
	���� ������ ��쿡�� extern Ű���带 �����ص�
	�ڵ����� extern������ �ȴ�. �׷��ٸ� �� ���� ������
	���������� �����ؼ� ����� ������ �ٸ� ���Ͽ��� �����ϴ°���
	���� ���� ����϶� static�� �����ϸ�ȴ�.
	*/

	inline LRESULT CALLBACK WndProc(
		HWND handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{


		ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam);

		//�޽����� ó���� ���̴�.
		//Switch�� ����ؼ� �����Ұ��̴�.
		switch (message)
		{
			//â�� �ݰų� â�� �ı��ɋ�.
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			//�ٸ��� �޽����϶��� ����� �´�.
			return DefWindowProc(handle, message, wParam, lParam);
			//
		}
		//��ȯ���� ������ ��ȯ.
		return 0;
	}

	//�����츦 ������ִ� �Լ�.
	inline void Create(HINSTANCE hInstance, const UINT& width, const UINT& height)
	{
		//�߰�
		g_instance = hInstance;

		//������ ���� Ŭ����.
		WNDCLASSEX wnd_class;
		//���� Ȯ��� ����� �����̴�.
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		//static_cast = ĳ��Ʈ �ϴ°�. ����ȯ.
		//GetStockObject = �ü���� �������ֹǷ� �Ϻη� ������ �ʾƵ�
		//�������� ����� �� ������ ����� �ı� �� �ʿ䵵 ����. 
		//GetStockObject��ȯ���� GDI�� ����ȯ ���ش�.
		wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wnd_class.hIcon = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hIconSm = LoadIcon(nullptr, IDI_ERROR); //Sm = ������
		wnd_class.hInstance = g_instance; //����//hInstance;
		wnd_class.lpfnWndProc = WndProc;
		/*
		CALLBACK / calling Convenion
		�Լ� ȣ�� ��Ģ, �޸𸮸� ���糪 ó���ϴ� ����� �ǹ��Ѵ�.
		*/
		wnd_class.lpszClassName = "D2DHello";
		wnd_class.lpszMenuName = nullptr;
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		//ȭ�� ũ�Ⱑ ���Ұ�� �����츦 �ٽ� �׸���.
		wnd_class.cbSize = sizeof(WNDCLASSEX);

		auto check = RegisterClassEx(&wnd_class);
		//auto ��� C++���� �߰��� ���.
		/*
		auto�� Ȱ���ϸ� ������ ���� �����Ϸ��� �˾Ƽ� �������ش�.
		��� auto�� ����Ǵ� ������ ����� ���ÿ� �ʱ�ȭ �Ǿ���Ѵٴ°� �����̴�.
		*/
		assert(check != 0, L"RegisterClassEx ����!");
		//true�Ͻ� �������� false�Ͻ� �ߴܵȴ�.

		g_handle = CreateWindowExW(
			WS_EX_APPWINDOW,
			L"D2DHello",
			L"D2DHello",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<int>(width),
			static_cast<int>(height),
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		assert(g_handle != nullptr);
	}

	//ȭ�� ����.
	inline void Show()
	{
		//�ֻ�ȭ������
		SetForegroundWindow(g_handle);
		SetFocus(g_handle);
		ShowCursor(TRUE);
		//â�� ��� �Ұ��̳� �ִ�ȭ �ּ�ȭ ���..
		ShowWindow(g_handle, SW_NORMAL);
		//����� ������ ������Ʈ ���ش�.
		UpdateWindow(g_handle);
	}

	inline bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		/*
		�޸� ������ 0x00���� ä��� ��ũ��(�Լ��� �ƴ�)
		*/
		//�� �����Ӹ��� �Լ��� ����Ǹ� �޽��� ť�� �˻��ϰ� �׻� ��ȯ�� 1�� ��.
		//��� ���°� ����. - �񵿱� ( ��û�� ���� �� ����ʹ� ������� ��������� �����ϴ� ���)
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.message != WM_QUIT;
	}

	inline void Destroy()
	{
		DestroyWindow(g_handle);
		UnregisterClass("D2DHello", g_instance);
		//���� ����� ������ų			//������
	}

	inline const UINT GetWidth()
	{
		RECT rect;
		GetClientRect(g_handle, &rect);
		return static_cast<UINT>(rect.right - rect.left);
	}

	inline const UINT GetHeight()
	{
		RECT rect;
		GetClientRect(g_handle, &rect);
		return static_cast<UINT>(rect.bottom - rect.top);
	}
}
