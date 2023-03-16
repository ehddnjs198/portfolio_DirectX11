#include "stdafx.h"
#include "cCanvas.h"
#include "Graphic/Graphic.h"

#include "Mesh/cMesh.h"
#include "Shader/ShaderTexture/cShaderTexture.h"
#include "Canvas/Image/cImage.h"

#include "Input/cInput.h"
#include "Ray/cRay.h"
#include "Canvas/UI/Image/cUIImage.h"
#include "Canvas/UI/Button/cUIButton.h"

#include<math.h>
#include<string>
#include<stdlib.h>

#include "imGui/imgui.h"

cCanvas::cCanvas()
{
}


cCanvas::~cCanvas()
{
	
	if(m_pUIBase)
		m_pUIBase->Destroy();

	SAFE_RELEASE(m_depthDisabledStencilState);
}

void cCanvas::Initialize(float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// 2D 렌더링을위한 직교 투영 행렬을 만듭니다
	D3DXMatrixOrthoLH(&m_orthoMatrix, screenWidth, screenHeight, screenNear, screenDepth);

	// 이제 2D 렌더링을위한 Z 버퍼를 끄는 두 번째 깊이 스텐실 상태를 만듭니다. 유일한 차이점은
	// DepthEnable을 false로 설정하면 다른 모든 매개 변수는 다른 깊이 스텐실 상태와 동일합니다.
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	Graphic::GetInstance().GetDevice()->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);

	D3DXMatrixLookAtLH(&m_UIView,
		&D3DXVECTOR3(0, 0,-1),
		&D3DXVECTOR3(0, 0, 1),
		&D3DXVECTOR3(0, 1, 0));

	for (int i = 0; i < 5; i++)
	{
		UiButton[i] = new cUIImage();
		UiButton[i]->m_Position = D3DXVECTOR3(0, 0, 0);
		////////////////////////////////////////////////
		/*유니티 버튼 구간*/

		string str = "Resources/UnityButton/temp";
		
		string num = to_string(i + 1 );
		string str1 = ".png";

		UiButton[i]->Initialize(str+num+str1, D3DXVECTOR3(20, 30, 0), 129*1.2, 22*1.2);
		UiButton[i]->m_pDelegate = this;
		m_pUIBase = UiButton[i];
	}

	//////////////////////////////////////start 버튼
	for (int i = 0; i < 3; i++)
	{
		UiStartButton[i] = new cUIImage();
		UiStartButton[i]->m_Position = D3DXVECTOR3(0, 0, 0);

		string str = "Resources/UnityUi/temp";

		string num = to_string(i + 1);
		string str1 = ".png";

		UiStartButton[i]->Initialize(str + num + str1, D3DXVECTOR3(560, 30, 0), 96*1.2, 22*1.2);
		UiStartButton[i]->m_pDelegate = this;
		m_pUIStartBase = UiStartButton[i];
	}

	m_pUIStartBase = UiStartButton[0];

	UiColor[0] = new cUIImage();
	UiColor[0]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiColor[0]->Initialize("Resources/UnityButton/Red.png", D3DXVECTOR3(20, 30, 0), 129 * 1.2/4, 22 * 1.2);
	UiColor[0]->m_pDelegate = this;
	m_pUcolor[0] = UiColor[0];

	UiColor[1] = new cUIImage();
	UiColor[1]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiColor[1]->Initialize("Resources/UnityButton/Blue.png", D3DXVECTOR3(20+ 129 * 1.2 / 4, 30, 0), 129 * 1.2/4, 22 * 1.2);
	UiColor[1]->m_pDelegate = this;
	m_pUcolor[1] = UiColor[1];
	
	UiColor[2] = new cUIImage();
	UiColor[2]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiColor[2]->Initialize("Resources/UnityButton/Black.png", D3DXVECTOR3(20 + 129 * 1.2 / 4*2, 30, 0), 129 * 1.2 / 4, 22 * 1.2);
	UiColor[2]->m_pDelegate = this;
	m_pUcolor[2] = UiColor[2];

	UiColor[3] = new cUIImage();
	UiColor[3]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiColor[3]->Initialize("Resources/UnityButton/Green.png", D3DXVECTOR3(20 + 129 * 1.2 / 4*3, 30, 0), 129 * 1.2 / 4, 22 * 1.2);
	UiColor[3]->m_pDelegate = this;
	m_pUcolor[3] = UiColor[3];

	////////////////////////////////////////////////

	/*UiStartColor[0] = new cUIImage();
	UiStartColor[0]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiStartColor[0]->Initialize("Resources/UnityUi/Red.png", D3DXVECTOR3(560, 30, 0), 96 * 1.2/3, 22 * 1.2);
	UiStartColor[0]->m_pDelegate = this;
	m_pUStartcolor[0] = UiStartColor[0];

	UiStartColor[1] = new cUIImage();
	UiStartColor[1]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiStartColor[1]->Initialize("Resources/UnityUi/Blue.png", D3DXVECTOR3(560+ 96 * 1.2/3, 30, 0), 96 * 1.2/3, 22 * 1.2);
	UiStartColor[1]->m_pDelegate = this;
	m_pUStartcolor[1] = UiStartColor[1];
	
	UiStartColor[2] = new cUIImage();
	UiStartColor[2]->m_Position = D3DXVECTOR3(0, 0, 0);
	UiStartColor[2]->Initialize("Resources/UnityUi/Green.png", D3DXVECTOR3(560+ 96 * 1.2/3*2, 30, 0), 96 /3* 1.2/3, 22 * 1.2);
	UiStartColor[2]->m_pDelegate = this;
	m_pUStartcolor[2] = UiStartColor[2];*/


}

void cCanvas::Update(D3DXMATRIX world, D3DXMATRIX view)
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.MouseDown[0])
	{
		POINT p = cInput::GetInstance().getMousePosition();


		if (m_pUcolor[0])
		{
			BOX->red = true;
			BOX->blue = false;
			BOX->black = false;
			BOX->green = false;

			m_pUcolor[0]->RayPickingRect(p);
		}

		if (m_pUcolor[1])
		{
			BOX->red = false;
			BOX->blue = true;
			BOX->black = false;
			BOX->green = false;

			m_pUcolor[1]->RayPickingRect(p);
		}

		if (m_pUcolor[2])
		{
			BOX->red = false;
			BOX->blue = false;
			BOX->black = true;
			BOX->green = false;

			m_pUcolor[2]->RayPickingRect(p);
		}

		if (m_pUcolor[3])
		{
			BOX->red = false;
			BOX->blue = false;
			BOX->black = false;
			BOX->green = true;

			m_pUcolor[3]->RayPickingRect(p);
		}

		/////////////////////
		/*if (m_pUStartcolor[0])
		{
			BOX->startRed = true;
			BOX->startBlue = false;
			BOX->startGreen = false;

			m_pUcolor[0]->RayPickingRect(p);
		}

		if (m_pUStartcolor[1])
		{
			BOX->startRed = false;
			BOX->startBlue = true;
			BOX->startGreen = false;

			m_pUStartcolor[1]->RayPickingRect(p);
		}
		if (m_pUStartcolor[2])
		{
			BOX->startRed = false;
			BOX->startBlue = false;
			BOX->startGreen = true;

			m_pUStartcolor[2]->RayPickingRect(p);
		}
*/
	}

	
	if (BOX->getKeyboardEvent()[1] || BOX->clickRed)
	{

		m_pUIBase = UiButton[1];

	}
	//w
	if (BOX->getKeyboardEvent()[2] || BOX->clickBlue)
	{
		m_pUIBase = UiButton[2];

	}
	//e
	if (BOX->getKeyboardEvent()[3] || BOX->clickBlack)
	{
		m_pUIBase = UiButton[3];

	}
	//r
	if (BOX->getKeyboardEvent()[4] || BOX->clickGreen)
	{
		m_pUIBase = UiButton[4];

	}
	///////////////////////////수정요망!!@#!@#
	if (KEYMANAGER->isStayKeyDown('Z'))
	{
		m_pUIStartBase = UiStartButton[0];
		BOX->isAnimation = false;
	}
	else if (KEYMANAGER->isStayKeyDown('X'))
	{
		m_pUIStartBase = UiStartButton[1];
		BOX->isAnimation = true;

	}

	if (KEYMANAGER->isStayKeyDown('C'))
	{
		m_pUIStartBase = UiStartButton[2];
		BOX->isAnimation = false;
	}

	m_pUcolor[0] = UiColor[0];
	m_pUcolor[1] = UiColor[1];
	m_pUcolor[2] = UiColor[2];
	m_pUcolor[3] = UiColor[3];

	

	if(m_pUIBase)
		m_pUIBase->Update(m_UIView, m_orthoMatrix);

	if (m_pUcolor[0])
		m_pUcolor[0]->Update(m_UIView, m_orthoMatrix);

	if (m_pUcolor[1])
		m_pUcolor[1]->Update(m_UIView, m_orthoMatrix);

	if (m_pUcolor[2])
		m_pUcolor[2]->Update(m_UIView, m_orthoMatrix);

	if (m_pUcolor[3])
		m_pUcolor[3]->Update(m_UIView, m_orthoMatrix);

	///////////////////////////
	if (m_pUIStartBase)
		m_pUIStartBase->Update(m_UIView, m_orthoMatrix);

}

void cCanvas::Render()
{
	//뎁스를 끄고.
	Graphic::GetInstance().GetDeviceContext()->OMSetDepthStencilState(m_depthDisabledStencilState, 1);

	m_pUcolor[0]->Render();
	m_pUcolor[1]->Render();
	m_pUcolor[2]->Render();
	m_pUcolor[3]->Render();
	m_pUIBase->Render();


	m_pUIStartBase->Render();


	Graphic::GetInstance().TurnZBufferOn();
}

void cCanvas::OnClick(cUIBase * pSender)
{
	//std::cout << "버튼을 눌렀다" << std::endl;
}
