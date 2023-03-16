#pragma once

#include "stdafx.h"


class Setting final
{
private:
	Setting() = default; //���C++���� �߰��� ���. �����ҽ� ���ص���.
	//Setting() { }
public:
	~Setting() = default;

private:
	HWND m_handle;
	float m_fWidth;
	float m_fHeight;
public:
	//�����ϰ� ���� ���� �̱���, �����ͷε� �ص���.
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