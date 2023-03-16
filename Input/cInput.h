#pragma once

#ifndef _INPUTCLASS_H_ 
#define _INPUTCLASS_H_

//#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class cInput
{
private:
	cInput();
public:

	static cInput& GetInstance()
	{
		static cInput instance;
		return instance;
	}
public:			// 인터페이스

	//! 상태를 갱신한다
	void update(void);

public:			// getter
	//! 마우스 휠을 반환한다
	int getMouseWheel(void);
	//! 마우스 위치를 반환한다
	POINT getMousePosition(void);

public:			// public 함수

	//! 싱글턴
	//DECLARE_SINGLETON(CInputManager);

	//! 초기화
	void init(HINSTANCE hinstance);

	//! 키 눌림 여부를 검사한다
	bool isKeyDown(int a_nKeyCode);

	//! 키 입력 여부를 검사한다
	bool isKeyPressed(int a_nKeyCode);

	//! 키 입력 종료 여부를 검사한다
	bool isKeyReleased(int a_nKeyCode);

	/*
	마우스 버튼 인덱스
	왼쪽 클릭 : 0
	오른쪽 클릭 : 1
	중앙 클릭 : 2
	*/
	//! 마우스 눌림 여부를 검사한다
	bool isMouseDown(const int a_nButtonIndex);

	//! 마우스를 눌렀을 경우
	bool isMousePressed(const int a_nButtonIndex);

	//! 마우스를 땠을 경우
	bool isMouseReleased(const int a_nButtonIndex);

private:			// 생성자, 소멸자
	//! 소멸자
	~cInput(void);
private:			// private 변수

	BYTE m_anKeyboardStates[UCHAR_MAX + 1];
	BYTE m_anPrevKeyboardStates[UCHAR_MAX + 1];

	DIMOUSESTATE m_stMouseState;
	DIMOUSESTATE m_stPrevMouseState;

	LPDIRECTINPUT8 m_pDirectInput = nullptr;
	LPDIRECTINPUTDEVICE8 m_pKeyboardDevice = nullptr;
	LPDIRECTINPUTDEVICE8 m_pMouseDevice = nullptr;
};

#endif