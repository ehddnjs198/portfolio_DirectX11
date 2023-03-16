#pragma once

/*
2D 이미지들을 나타낼 것이다.
*/

class cImage
{
private:
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

	RECT m_rect;

	class cShaderTexture * m_pShaderTexture = nullptr;

public:
	enum Type
	{
		Simple = 0,
		Sliced = 1,
		Tiled = 2,
		Filled = 3
	};
	enum FillMethod
	{
		Horizontal = 0,
		Vertical = 1,
		Radial90 = 2,
		Radial180 = 3,
		Radial360 = 4
	};
public:
	cImage();
	~cImage();

	void Initialize(const std::string & shaderResource);
	void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	void Render();

	void SetFileAmount(float value);

	bool RayPicking(class cRay ray);
	bool RayPickingRect(POINT p);
public:
	float fillAmount = 1;
	Type type = Type::Simple;
	FillMethod fillMethod;
};

