#include "stdafx.h"
#include "cUIButton.h"
#include "Input/cInput.h"

cUIButton::cUIButton()
{
}


cUIButton::~cUIButton()
{
}

void cUIButton::Update(D3DXMATRIX view, D3DXMATRIX proj)
{
}

cUIButtonDelegate * cUIButton::GetDelegate() const
{
	return m_pDelegate;
}
