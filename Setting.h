#pragma once

#include "stdafx.h"


class Setting final
{
private:
	Setting() = default; //모던C++에서 추가된 기능. 불편할시 안해도됨.
	//Setting() { }
public:
	~Setting() = default;

private:
	HWND m_handle;
	float m_fWidth;
	float m_fHeight;
public:
	//간단하게 만든 정적 싱글톤, 포인터로도 해도됨.
	static Setting& GetInstance()
	{
		static Setting instance;
		return instance;
	}

	auto GetWindowHandle() const -> HWND { return m_handle; }
	void SetWindowHandle(HWND handle) { this->m_handle = handle; }

	auto GetWindowWidth() const -> const float& { return m_fWidth; }
	void SetWindowWidth(const float& width) { this->m_fWidth = width; }

	auto GetWindowHeight() const -> const float& { return m_fHeight; }
	void SetWindowHeight(const float& height) { this->m_fHeight = height; }

};