#pragma once

#include "UI/Button/cUIButton.h"



class cCanvas : public cUIButtonDelegate
{
public:
	cCanvas();
	~cCanvas();

	static cCanvas& GetInstance()
	{
		static cCanvas instance;
		return instance;
	}

public:
	void Initialize(float screenWidth, float screenHeight, float screenNear, float screenDepth);
	void Update(D3DXMATRIX world, D3DXMATRIX view);
	void Render();
private:
	D3DXMATRIX m_orthoMatrix;
	ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
public:
	//TEST용도. 
	D3DXMATRIX m_UIView;
	//class cMesh * m_pMesh = nullptr;
	//class cShaderTexture* m_pShaderTexture = nullptr;
	//class cImage * m_pImage = nullptr;
	//class cUIImage * m_pUIImage = nullptr;
	//class cUIGrid * m_pUIGrid = nullptr;

	class cUIBase * m_pUIBase = nullptr;
	class cUIBase* m_pUcolor[5];

	class cUIImage* UiButton[5];

	class cUIImage* UiColor[5];


	class cUIBase * m_pUIStartBase = nullptr;
	//class cUIBase* m_pUStartcolor[5];

	class cUIImage* UiStartButton[3];
	//class cUIImage* UiStartColor[3];

	bool isStart = false;

	// cUIButtonDelegate을(를) 통해 상속됨
	virtual void OnClick(cUIBase * pSender) override;

};

