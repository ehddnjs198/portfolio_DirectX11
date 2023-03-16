#pragma once

/*
UI의 기초가 될놈.
UI는 2D이니 API만들떄를 생각하자.
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

