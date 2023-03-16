#pragma once


struct ST__VERTEX_P
{
	D3DXVECTOR3 P;
};

struct ST__VERTEX_PT
{
	D3DXVECTOR3 P;
	D3DXVECTOR2 T;
};
struct ST__VERTEX_PC
{
	D3DXVECTOR3 P;
	D3DXCOLOR C;
};

class cMESH
{
public:
	virtual void Initialize() = 0;
	virtual UINT GetVertexSize() const = 0;
	virtual UINT GetInicesSize() const = 0;
	virtual UINT GetStructSize() const = 0;

	virtual const void* GetVertexReference() const = 0;
	virtual const void* GetInicesReference() const = 0;

	//////////???????/////////////////////
	virtual const D3DXVECTOR3 GetVertexPosition(UINT index) const = 0;
	virtual const UINT GetInicesValue(UINT index) const = 0;
};

//클래스로 만들까?...구조체로 할까...
class cMESH_QUAD : public cMESH
{
public:
	cMESH_QUAD();
	~cMESH_QUAD();
public:
	
public:
	const UINT m_vertexSize = 4;
	const UINT m_indicesSize = 6;
	//std::vector<ST__VERTEX_P> m_vecVertex;
	//T 써도 될꺼 같은ㄷ..?
	std::vector<ST__VERTEX_PT> m_vecVertex;
	std::vector<UINT> m_vecIndices;
public:

	// cMESH을(를) 통해 상속됨
	virtual void Initialize() override;

	virtual UINT GetVertexSize() const override;
	virtual UINT GetInicesSize() const override;
	virtual UINT GetStructSize() const override;

	virtual const void* GetVertexReference() const override;
	virtual const void* GetInicesReference() const override;

	virtual const D3DXVECTOR3 GetVertexPosition(UINT index) const override;
	virtual const UINT GetInicesValue(UINT index) const override;
};

class cMESH_BOX : public cMESH
{
public:
	cMESH_BOX();
	~cMESH_BOX();
public:
	//void Initialize();
public:
	const UINT m_vertexSize = 24;
	const UINT m_indicesSize = 36;
	std::vector<ST__VERTEX_PC> m_vecVertex;
	std::vector<UINT> m_vecIndices;
public:

	virtual void Initialize() override;
	virtual UINT GetVertexSize() const override;
	virtual UINT GetInicesSize() const override;
	virtual UINT GetStructSize() const override;

	virtual const void* GetVertexReference() const override;
	virtual const void* GetInicesReference() const override;

	virtual const D3DXVECTOR3 GetVertexPosition(UINT index) const override;
	virtual const UINT GetInicesValue(UINT index) const override;
};

class cMesh
{
public:
	cMesh();
	cMesh(cMESH *mesh);
	~cMesh();
public:
	void Update();
	void Render();

	
	cMESH * GetMeshInfo();
public:
	cMESH * m_pMeshInfo = nullptr;
private:
	ID3D11Buffer * m_pVertexBuffer = nullptr;
	ID3D11Buffer * m_Idices_Buffer = nullptr;

};

