#include "stdafx.h"
#include "cInput.h"
//
#include "Setting.h"
#include "Graphic/Graphic.h"
#include "Canvas/Image/cImage.h"
#include "Canvas/UI/Image/cUIImage.h"

#include "Window/Window.h"

cInput::cInput(void)
{
	ZeroMemory(m_anKeyboardStates, sizeof(m_anKeyboardStates));
	ZeroMemory(m_anPrevKeyboardStates, sizeof(m_anPrevKeyboardStates));

	ZeroMemory(&m_stMouseState, sizeof(m_stMouseState));
	ZeroMemory(&m_stPrevMouseState, sizeof(m_stPrevMouseState));
}

cInput::~cInput(void)
{
	// 디바이스의 권한을 반환한다
	m_pKeyboardDevice->Unacquire();
	SAFE_RELEASE(m_pKeyboardDevice);

	m_pMouseDevice->Unacquire();
	SAFE_RELEASE(m_pMouseDevice);

	SAFE_RELEASE(m_pDirectInput);
}

void cInput::init(HINSTANCE hinstance)
{
	//DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)& m_pDirectInput, NULL);
	DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)& m_pDirectInput, NULL);

	// initialize the keyboard
	m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL);
	m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboardDevice->SetCooperativeLevel(Window::g_handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboardDevice->Acquire();

	// initialize the keyboard
	m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL);
	m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	m_pMouseDevice->SetCooperativeLevel(Window::g_handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pMouseDevice->Acquire();
}

void cInput::update(void)
{
	//memcpy
	CopyMemory(&m_stPrevMouseState, &m_stMouseState, sizeof(m_stMouseState));
	CopyMemory(m_anPrevKeyboardStates, m_anKeyboardStates, sizeof(m_anKeyboardStates));

	m_pMouseDevice->GetDeviceState(sizeof(m_stMouseState), &m_stMouseState);
	m_pKeyboardDevice->GetDeviceState(sizeof(m_anKeyboardStates), m_anKeyboardStates);
}

int cInput::getMouseWheel(void)
{
	return m_stMouseState.lZ;
}

POINT cInput::getMousePosition(void)
{
	POINT stMousePosition;

	GetCursorPos(&stMousePosition);

	ScreenToClient(Setting::GetInstance().GetWindowHandle(), &stMousePosition);

	return stMousePosition;
}

bool cInput::isKeyDown(int a_nKeyCode)
{
	return m_anKeyboardStates[a_nKeyCode] & 0x80;
}

bool cInput::isKeyPressed(int a_nKeyCode)
{
	return (m_anKeyboardStates[a_nKeyCode] & 0x80) &&
		!(m_anPrevKeyboardStates[a_nKeyCode] & 0x80);
}

bool cInput::isKeyReleased(int a_nKeyCode)
{
	return !(m_anKeyboardStates[a_nKeyCode] & 0x80) &&
		(m_anPrevKeyboardStates[a_nKeyCode] & 0x80);
}
bool cInput::isMouseDown(const int a_nButtonIndex)
{
	return m_stMouseState.rgbButtons[a_nButtonIndex] & 0x80;
}

bool cInput::isMousePressed(const int a_nButtonIndex)
{
	return !(m_stPrevMouseState.rgbButtons[a_nButtonIndex] & 0x80) &&
		(m_stMouseState.rgbButtons[a_nButtonIndex] & 0x80);
}

bool cInput::isMouseReleased(const int a_nButtonIndex)
{
	return !(m_stMouseState.rgbButtons[a_nButtonIndex] & 0x80) &&
		(m_stPrevMouseState.rgbButtons[a_nButtonIndex] & 0x80);
}