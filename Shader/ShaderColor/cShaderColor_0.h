#pragma once

#include "Shader/cShaderBase.h"



class cShaderColor_0 : public cShaderBase
{
public:
	cShaderColor_0();
	virtual ~cShaderColor_0();

public:
	virtual void Initialize(const std::string &shaderFile);
	virtual void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj) override;
	virtual void Render(UINT drawCount) override;
protected:
	virtual void InitializeInputLayer(ID3DBlob * m_VS_Blob) override;
public:
	D3DXCOLOR m_Color;
private:
	ID3D11Buffer *m_lp_PS_Color_Buffer = nullptr;
};

