#include "stdafx.h"
#include "cMesh.h"
#include "Graphic/Graphic.h"

cMesh::cMesh()
{
}
cMesh::cMesh(cMESH * mesh)
{
	m_pMeshInfo = mesh;
	m_pMeshInfo->Initialize();

	{//Vertex Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//어느 스테이지에서 쓸것이냐.
		desc.ByteWidth = m_pMeshInfo->GetStructSize() * m_pMeshInfo->GetVertexSize();

		D3D11_SUBRESOURCE_DATA sub_data;
		//CPU데이터를 GPU에 연결 할 수 있다.
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));

		//버퍼 연결.
		sub_data.pSysMem = m_pMeshInfo->GetVertexReference();

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc,	//버퍼에 대한 설명서.
			&sub_data, //초기화 데이터.
			&m_pVertexBuffer //버텍스 버퍼.
		);

		assert(SUCCEEDED(hr));
	}

	{//Index Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(UINT) * m_pMeshInfo->GetInicesSize();
		//
		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = m_pMeshInfo->GetInicesReference();
		//
		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, &sub_data, &m_Idices_Buffer
		);

		assert(SUCCEEDED(hr));
	}
}
cMesh::~cMesh()
{
	SAFE_RELEASE(m_Idices_Buffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_DELETE(m_pMeshInfo);
}

void cMesh::Update()
{

}

void cMesh::Render()
{
	UINT stride = m_pMeshInfo->GetStructSize();
	UINT offset = 0;

	Graphic::GetInstance().GetDeviceContext()->IASetVertexBuffers
	(
		0,
		1,
		&m_pVertexBuffer,
		&stride,
		&offset
	);
	//
	Graphic::GetInstance().GetDeviceContext()->IASetIndexBuffer
	(
		m_Idices_Buffer,
		DXGI_FORMAT_R32_UINT,
		0
	);

	Graphic::GetInstance().GetDeviceContext()->IASetPrimitiveTopology
	(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
}


cMESH * cMesh::GetMeshInfo()
{
	return m_pMeshInfo;
}

cMESH_QUAD::cMESH_QUAD()
{
}
cMESH_QUAD::~cMESH_QUAD()
{
}

void cMESH_QUAD::Initialize()
{
	D3DXVECTOR3 vec[4] =
	{
		//
		//(1)--------(3)
//			/ 0,0,0 /
		//(0)--------(2)
		//
		//D3DXVECTOR3(-0.5f,	0,	-0.5f),	//0
		//D3DXVECTOR3(-0.5f,	0,	0.5f),	//1
		//D3DXVECTOR3(0.5f,	0,	-0.5f),	//2
		//D3DXVECTOR3(0.5f,	0,	0.5f),	//3

		D3DXVECTOR3(-0.5f,	-0.5f,0),	//0
		D3DXVECTOR3(-0.5f,	0.5f,0),	//1
		D3DXVECTOR3(0.5f,	-0.5f,0),	//2
		D3DXVECTOR3(0.5f,	0.5f,0),	//3
	};

	D3DXVECTOR2 uv[4] =
	{
		//
		//(1)--------(3)
//			/ 0,0,0 /
		//(0)--------(2)
		//
		D3DXVECTOR2(0.0f,	1),	//0
		D3DXVECTOR2(0.f,	0),	//1
		D3DXVECTOR2(1.f,	1.f),	//2
		D3DXVECTOR2(1.f,	0),	//3
	};

	for (int i = 0; i < m_vertexSize; ++i)
	{
		ST__VERTEX_PT vertex;
		vertex.P = vec[i];
		vertex.T = uv[i];
		m_vecVertex.push_back(vertex);
	}

	UINT Indices[6] =
	{
		//0,3,1,
		//0,2,3,
		0, 1, 3,
		0, 3, 2,
	};

	for (int i = 0; i < m_indicesSize; ++i)
	{
		m_vecIndices.push_back(Indices[i]);
	}
}

UINT cMESH_QUAD::GetVertexSize() const
{
	return m_vertexSize;
}

UINT cMESH_QUAD::GetInicesSize() const
{
	return m_indicesSize;
}

UINT cMESH_QUAD::GetStructSize() const
{
	return sizeof(ST__VERTEX_PT);
}

const void * cMESH_QUAD::GetVertexReference() const
{
	return &m_vecVertex[0];
}

const void * cMESH_QUAD::GetInicesReference() const
{
	return &m_vecIndices[0];
}

const D3DXVECTOR3 cMESH_QUAD::GetVertexPosition(UINT index) const
{
	return m_vecVertex[index].P;
}

const UINT cMESH_QUAD::GetInicesValue(UINT index) const
{
	return m_vecIndices[index];
}

////////////////////////////////////////////////////////////////

cMESH_BOX::cMESH_BOX()
{
}
cMESH_BOX::~cMESH_BOX()
{
}

void cMESH_BOX::Initialize()
{
	D3DXVECTOR3 vec[8] =
	{
		//
		//(1)--------(3)
//			/ 0,0,0 /
		//(0)--------(2)
		//
		D3DXVECTOR3(-0.5f,	-0.5f,	-0.5f),	//0
		D3DXVECTOR3(-0.5f,	0.5f,	-0.5f),	//1
		D3DXVECTOR3(0.5f,	-0.5f,	-0.5f),	//2
		D3DXVECTOR3(0.5f,	0.5f,	-0.5f),	//3

		D3DXVECTOR3(-0.5f,	-0.5f,	0.5f),	//0
		D3DXVECTOR3(-0.5f,	0.5f,	0.5f),	//1
		D3DXVECTOR3(0.5f,	-0.5f,	0.5f),	//2
		D3DXVECTOR3(0.5f,	0.5f,	0.5f),	//3
	};

	ST__VERTEX_PC vertex;

	vertex.P = vec[0];
	vertex.C = D3DXCOLOR(1,0,0,1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[1];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[2];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[3];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);
	///////////////////////////////////////////
	vertex.P = vec[4];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[5];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[6];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[7];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);
	///////////////////////////////////////////
	
	vertex.P = vec[0];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[1];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[4];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[5];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	
	//오른쪽
	vertex.P = vec[2];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[3];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[6];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[7];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	//밑면
	vertex.P = vec[0];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[4];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[2];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[6];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);
	//윗면
	vertex.P = vec[1];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[5];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[3];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	vertex.P = vec[7];
	vertex.C = D3DXCOLOR(1, 0, 0, 1);
	m_vecVertex.push_back(vertex);

	UINT Indices[36] =
	{
		//전면 시계방향
		0,1,3,	3,2,0	,
		//후면 시계방향
		4,7,5,	4,6,7	,
		//왼쪽 시계방향
		8,10,11, 8,11,9,
		//오른쪽 시계방향
		12,13,15, 12,15,14,
		//밑면 시계방향
		16,18,19, 16,19,17,
		//윗면 시계방향.
		20,21,23, 20,23,22
	};

	for (int i = 0; i < m_indicesSize; ++i)
	{
		m_vecIndices.push_back(Indices[i]);
	}
}

UINT cMESH_BOX::GetVertexSize() const
{
	return m_vertexSize;
}

UINT cMESH_BOX::GetInicesSize() const
{
	return m_indicesSize;
}

UINT cMESH_BOX::GetStructSize() const
{
	return sizeof(ST__VERTEX_PC);
}

const void * cMESH_BOX::GetVertexReference() const
{
	return &m_vecVertex[0];
}

const void * cMESH_BOX::GetInicesReference() const
{
	return &m_vecIndices[0];
}

const D3DXVECTOR3 cMESH_BOX::GetVertexPosition(UINT index) const
{
	return m_vecVertex[index].P;
}

const UINT cMESH_BOX::GetInicesValue(UINT index) const
{
	return m_vecIndices[index];
}


