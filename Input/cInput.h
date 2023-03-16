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
public:			// �������̽�

	//! ���¸� �����Ѵ�
	void update(void);

public:			// getter
	//! ���콺 ���� ��ȯ�Ѵ�
	int getMouseWheel(void);
	//! ���콺 ��ġ�� ��ȯ�Ѵ�
	POINT getMousePosition(void);

public:			// public �Լ�

	//! �̱���
	//DECLARE_SINGLETON(CInputManager);

	//! �ʱ�ȭ
	void init(HINSTANCE hinstance);

	//! Ű ���� ���θ� �˻��Ѵ�
	bool isKeyDown(int a_nKeyCode);

	//! Ű �Է� ���θ� �˻��Ѵ�
	bool isKeyPressed(int a_nKeyCode);

	//! Ű �Է� ���� ���θ� �˻��Ѵ�
	bool isKeyReleased(int a_nKeyCode);

	/*
	���콺 ��ư �ε���
	���� Ŭ�� : 0
	������ Ŭ�� : 1
	�߾� Ŭ�� : 2
	*/
	//! ���콺 ���� ���θ� �˻��Ѵ�
	bool isMouseDown(const int a_nButtonIndex);

	//! ���콺�� ������ ���
	bool isMousePressed(const int a_nButtonIndex);

	//! ���콺�� ���� ���
	bool isMouseReleased(const int a_nButtonIndex);

private:			// ������, �Ҹ���
	//! �Ҹ���
	~cInput(void);
private:			// private ����

	BYTE m_anKeyboardStates[UCHAR_MAX + 1];
	BYTE m_anPrevKeyboardStates[UCHAR_MAX + 1];

	DIMOUSESTATE m_stMouseState;
	DIMOUSESTATE m_stPrevMouseState;

	LPDIRECTINPUT8 m_pDirectInput = nullptr;
	LPDIRECTINPUTDEVICE8 m_pKeyboardDevice = nullptr;
	LPDIRECTINPUTDEVICE8 m_pMouseDevice = nullptr;
};

#endif