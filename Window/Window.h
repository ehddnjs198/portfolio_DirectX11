#pragma once

#include "stdafx.h"

//#include "imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
	/*
	네임 스페이스를 사용하는 이유는 기본적으로 C++ 사용하는데
	객체지향이라고 부른다., 객체지향이라 부르는것은 간단하게 말하면
	정리정돈하게 한다는것, 재사용성을 높이는것이다.
	전역 변수는 관리가 잘 안된다. 그래서 묶었다.
	보통 C++에서는 전역변수, 함수를 만드는것은 namespacew로 묶어주는게 편하다.
	*/
{
	static HINSTANCE g_instance;
	static HWND g_handle;
	/*
	전역 변수의 경우에는 extern 키워드를 생략해도
	자동으로 extern선언이 된다. 그렇다면 한 파일 내에서
	전역변수로 선언해서 쓰고는 싶은대 다른 파일에서 참조하는것은
	막고 싶을 경우일때 static를 선언하면된다.
	*/

	inline LRESULT CALLBACK WndProc(
		HWND handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{


		ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam);

		//메시지를 처리할 것이다.
		//Switch를 사용해서 관리할것이다.
		switch (message)
		{
			//창을 닫거나 창이 파괴될떄.
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			//다른값 메시지일때은 여기로 온다.
			return DefWindowProc(handle, message, wParam, lParam);
			//
		}
		//반환값이 있으니 반환.
		return 0;
	}

	//윈도우를 만들어주는 함수.
	inline void Create(HINSTANCE hInstance, const UINT& width, const UINT& height)
	{
		//추가
		g_instance = hInstance;

		//윈도우 설계 클래스.
		WNDCLASSEX wnd_class;
		//좀더 확장된 기능을 쓸것이다.
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		//static_cast = 캐스트 하는것. 형변환.
		//GetStockObject = 운영체제가 제공해주므로 일부러 만들지 않아도
		//언제든지 사용할 수 있으며 사용후 파괴 할 필요도 없다. 
		//GetStockObject반환형이 GDI라서 형변환 해준다.
		wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wnd_class.hIcon = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hIconSm = LoadIcon(nullptr, IDI_ERROR); //Sm = 작은거
		wnd_class.hInstance = g_instance; //수정//hInstance;
		wnd_class.lpfnWndProc = WndProc;
		/*
		CALLBACK / calling Convenion
		함수 호출 규칙, 메모리를 적재나 처리하는 방식을 의미한다.
		*/
		wnd_class.lpszClassName = "D2DHello";
		wnd_class.lpszMenuName = nullptr;
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		//화면 크기가 변할경우 윈도우를 다시 그리고.
		wnd_class.cbSize = sizeof(WNDCLASSEX);

		auto check = RegisterClassEx(&wnd_class);
		//auto 모던 C++에서 추가된 기능.
		/*
		auto를 활용하면 데이터 형을 컴파일러가 알아서 지정해준다.
		대신 auto로 선언되는 변수는 선언과 동시에 초기화 되어야한다는게 조건이다.
		*/
		assert(check != 0, L"RegisterClassEx 에러!");
		//true일시 지나가고 false일시 중단된다.

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

	//화면 띄우기.
	inline void Show()
	{
		//최상화면으로
		SetForegroundWindow(g_handle);
		SetFocus(g_handle);
		ShowCursor(TRUE);
		//창을 어떻게 할것이냐 최대화 최소화 등등..
		ShowWindow(g_handle, SW_NORMAL);
		//변경된 정보를 업데이트 해준다.
		UpdateWindow(g_handle);
	}

	inline bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		/*
		메모리 영역을 0x00으로 채우는 매크로(함수가 아님)
		*/
		//매 프레임마다 함수가 실행되면 메시지 큐를 검사하고 항상 반환을 1로 함.
		//대기 상태가 없음. - 비동기 ( 요청을 보낸 후 결과와는 상관없이 다음방식이 동작하는 방식)
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
		//내가 등록을 해제시킬			//실제값
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
