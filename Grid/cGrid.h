#pragma once




class cGrid
{
private:
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

public:
	cGrid(const int &sizeX, const int &sizeY);
	~cGrid();
public:

	int vertexSize = 0;
	int IdicesSize = 0;

	Vertex * m_pVertexs = nullptr;
	ID3D11Buffer * m_pVertexBuffer = nullptr;
	ID3D11InputLayout * m_pInputLayer = nullptr;

	//VS
	ID3D11VertexShader * m_Vertex_Shader = nullptr;
	ID3DBlob* m_VS_Blob = nullptr;

	//PS
	ID3D11PixelShader * m_Pixel_Shader = nullptr;
	ID3DBlob * m_PS_Blob = nullptr;

	//인덱스 버퍼를 만들기 위함.
	UINT *m_idices = nullptr;
	ID3D11Buffer * m_Idices_Buffer = nullptr;
	
	TRANSFORM_DATA m_cup_Buffer;
	ID3D11Buffer * m_gpu_Buffer;
	ID3D11RasterizerState * m_Rasterizer_State = nullptr;

	///
	D3DXVECTOR3 size = D3DXVECTOR3(1, 1, 1);
	D3DXVECTOR3 offset = D3DXVECTOR3(0, 0, 0);
public:
	void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	//디버깅 목적?
	void Render();



};

