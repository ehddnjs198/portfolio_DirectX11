#pragma once

#include "Canvas/UI/cUIBase.h"

class cUIButtonDelegate
{
public:
	virtual void OnClick(cUIBase* pSender) = 0;
};

class cUIButton : public cUIBase
{
private:
	enum eButtonStatus
	{
		E_NORMAL,
		E_MOUSEOVER,
		E_SELECTED,
		E_COUNT
	};
	eButtonStatus		m_eButtonStatus;
public:
	cUIButton(void);
	virtual ~cUIButton(void);

	virtual void Update(D3DXMATRIX view, D3DXMATRIX proj) override;
	//virtual void Render() override;
	cUIButtonDelegate*  m_pDelegate = nullptr;// Delegate);
	cUIButtonDelegate* GetDelegate() const;


};

