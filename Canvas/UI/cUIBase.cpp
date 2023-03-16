#include "stdafx.h"
#include "cUIBase.h"

cUIBase::cUIBase()
{
}


cUIBase::~cUIBase()
{
}

void cUIBase::AddChild(cUIBase * child)
{
	//예외 처리
	//부모 설정
	child->m_pParent = this;

	m_vecChlid.push_back(child);
}

void cUIBase::Update(D3DXMATRIX view, D3DXMATRIX proj)
{
	D3DXMatrixIdentity(&m_matWorld);
	m_matWorld._41 = m_Position.x;
	m_matWorld._42 = m_Position.y;
	m_matWorld._43 = m_Position.z;
	
	if (m_pParent)
	{
		m_matWorld._41 += m_pParent->m_matWorld._41;
		m_matWorld._42 += m_pParent->m_matWorld._42;
		m_matWorld._43 += m_pParent->m_matWorld._43;
	}

	for(auto p : m_vecChlid)
	{
		p->Update(view, proj);
	}
}

void cUIBase::Render()
{
	for (auto p : m_vecChlid)
	{
		p->Render();
	}
}

void cUIBase::Destroy()
{
	for (auto p : m_vecChlid)
	{
		p->Destroy();
	}
}

bool cUIBase::RayPickingRect(POINT p)
{
	if (PtInRect(&m_Rect, p))
	{
		if (BOX->red)
		{
			BOX->clickRed = true;
			BOX->clickBlue = false;
			BOX->clickBlack = false;
			BOX->green = false;

		}
		if(BOX->blue)
		{
			BOX->clickRed = false;
			BOX->clickBlue = true;
			BOX->clickBlack = false;
			BOX->green = false;
		}
		if (BOX->black)
		{
			BOX->clickRed = false;
			BOX->clickBlue = false;
			BOX->clickBlack = true;
			BOX->green = false;
		}
		if (BOX->green)
		{
			BOX->clickRed = false;
			BOX->clickBlue = false;
			BOX->clickBlack = false;
			BOX->clickGreen = true;
		}

		

		return true;
	}

	for (auto obj : m_vecChlid)
	{
		obj->RayPickingRect(p);
	}

	return false;
}
