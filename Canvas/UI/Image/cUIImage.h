#pragma once

#include "Canvas/UI/cUIBase.h"
#include "Canvas/UI/Button/cUIButton.h"



class cUIImage : public cUIBase
{
public:
	cUIImage();
	virtual ~cUIImage();
public:
	//RECT m_Rect;
	//D3DXVECTOR2 m_Position;

	std::vector<cUIBase *> m_vecChlid;

	struct ST__VERTEX_PT
	{
		D3DXVECTOR3 P;
		D3DXVECTOR2 T;
	};
	std::vector<ST__VERTEX_PT> m_vecVertex;
	std::vector<UINT> m_vecIndices;
	const UINT m_vertexSize = 4;
	const UINT m_indicesSize = 6;
	ID3D11Buffer * m_pVertexBuffer = nullptr;
	ID3D11Buffer * m_Idices_Buffer = nullptr;
	class cShaderTexture * m_pShaderTexture = nullptr;

public:
	void Initialize(const std::string & shaderResource, D3DXVECTOR3 pos,  int width, int height);
	//void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);

	virtual void Update(D3DXMATRIX view, D3DXMATRIX proj) override;
	virtual void Render() override;
	//virtual bool RayPickingRect(POINT p) override;

	cUIButtonDelegate*  m_pDelegate = nullptr;// Delegate);
	cUIButtonDelegate* GetDelegate() const;
};

