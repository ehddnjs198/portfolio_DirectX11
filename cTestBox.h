#pragma once


struct Vertex
{
	D3DXVECTOR3 m_VecPosition;
	//x y z
	D3DXCOLOR m_Color;
};

struct TRANSFORM_DATA
{
	D3DXMATRIX m_Mat_Word;
	D3DXMATRIX m_Mat_View;
	D3DXMATRIX m_Mat_Projection;
};

class cTestBox
{
public:
	cTestBox(D3DXCOLOR color);
	~cTestBox();
public:

	class cTransform* m_pTransform = nullptr;

	Vertex * m_pVertexs = nullptr;
	ID3D11Buffer * m_pVertexBuffer = nullptr;

	UINT *m_idices = nullptr;
	ID3D11Buffer * m_Idices_Buffer = nullptr;

	ID3D11InputLayout * m_pInputLayer = nullptr;

	//VS
	ID3D11VertexShader * m_Vertex_Shader = nullptr;
	ID3DBlob* m_VS_Blob = nullptr;

	//PS
	ID3D11PixelShader * m_Pixel_Shader = nullptr;
	ID3DBlob * m_PS_Blob = nullptr;

	TRANSFORM_DATA m_cup_Buffer;
	ID3D11Buffer * m_gpu_Buffer;

	//RS
	ID3D11RasterizerState * m_Rasterizer_State = nullptr;

public:
	void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	//void Update(class cTransform* world, D3DXMATRIX view, D3DXMATRIX proj);
	void Render();
};

