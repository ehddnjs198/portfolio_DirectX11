#pragma once

/*
UI�� ���ʰ� �ɳ�.
UI�� 2D�̴� API���鋚�� ��������.
*/

class cUIBase
{
public:
	cUIBase();
	virtual ~cUIBase();

public:
	RECT m_Rect;
	D3DXVECTOR3 m_Position;
	cUIBase* m_pParent = nullptr;
	D3DXMATRIX m_matWorld;
	std::vector<cUIBase *> m_vecChlid;
public:
	virtual void AddChild(cUIBase * child);
	virtual void Update(D3DXMATRIX view, D3DXMATRIX proj);
	virtual void Render();
	virtual void Destroy();

	virtual bool RayPickingRect(POINT p);
//	void Initialize();
//	void Update();
};

