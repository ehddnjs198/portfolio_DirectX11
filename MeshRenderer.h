#pragma once
class SkinnedMesh;

class MeshRenderer
{
private:
	struct st_Transform_Data
	{
		D3DXMATRIX m_Mat_Word;
		D3DXMATRIX m_Mat_View;
		D3DXMATRIX m_Mat_Projection;
	};

	ID3D11Buffer* m_pVerticesBuffer = nullptr;
	ID3D11Buffer * m_pIndicesBuffer = nullptr;

	ID3D11InputLayout* m_pInputLayout = nullptr;

	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3DBlob* m_VS_Blob = nullptr;

	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3DBlob* m_PS_Blob = nullptr;

	st_Transform_Data m_stTransform_Data;
	ID3D11Buffer* m_Transform_Buffer;

	ID3D11RasterizerState* m_pRasterizerState = nullptr;



public:
	MeshRenderer() {};
	~MeshRenderer();

	bool Init();
	void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	void Render();
};

