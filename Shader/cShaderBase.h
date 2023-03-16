#pragma once

#include "Graphic/Graphic.h"

struct ST__VSTransform_Data
{
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
};


class cShaderBase
{
public:
	cShaderBase();
	virtual ~cShaderBase();
protected:
	ID3D11VertexShader * m_Vertex_Shader = nullptr;
	ID3D11PixelShader * m_Pixel_Shader = nullptr;
	ID3D11InputLayout * m_pInputLayer = nullptr;

	ST__VSTransform_Data m_st_VS_Data;
	ID3D11Buffer *m_lp_VS_Buffer = nullptr;

	ID3D11RasterizerState* m_lp_Rasterizer_State = nullptr;

public:
	virtual void Initialize(const std::string &shaderFile);
	virtual void Initialize(const std::string &shaderFile, const std::string &shaderResource);

	virtual void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	virtual void Render(UINT drawCount);

protected:
	virtual void InitializeInputLayer(ID3DBlob *m_VS_Blob) = 0;
};

