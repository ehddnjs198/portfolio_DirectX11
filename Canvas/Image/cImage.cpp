#include "stdafx.h"
#include "cImage.h"
#include "Graphic/Graphic.h"
#include "Shader/ShaderTexture/cShaderTexture.h"
#include "Ray/cRay.h"

cImage::cImage()
{
}


cImage::~cImage()
{
	SAFE_DELETE(m_pShaderTexture);
	SAFE_RELEASE(m_Idices_Buffer);
	SAFE_RELEASE(m_pVertexBuffer);
}

void cImage::Initialize(const std::string & shaderResource)
{
	D3DXVECTOR3 vec[4] =
	{
		D3DXVECTOR3(-0.5f,	-0.5f,0),	//0
		D3DXVECTOR3(-0.5f,	0.5f,0),	//1
		D3DXVECTOR3(0.5f,	-0.5f,0),	//2
		D3DXVECTOR3(0.5f,	0.5f,0),	//3
	};

	//사이즈 조정.
	for (int i = 0; i < 4; ++i)
	{
		vec[i] *= 100;
	}

	m_rect = 
	{
		static_cast<long>(vec[0].x),
		static_cast<long>(vec[1].y),

		100, 100
	};

	D3DXVECTOR2 uv[4] =
	{
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
		0, 1, 3,
		0, 3, 2,
	};

	for (int i = 0; i < m_indicesSize; ++i)
	{
		m_vecIndices.push_back(Indices[i]);
	}

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = sizeof(ST__VERTEX_PT) * m_vertexSize;

		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;


		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));

		//버퍼 연결.
		sub_data.pSysMem = &m_vecVertex[0];

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
		desc.ByteWidth = sizeof(UINT) * m_indicesSize;
		//
		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = &m_vecIndices[0];
		//
		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, &sub_data, &m_Idices_Buffer
		);

		assert(SUCCEEDED(hr));
	}

	m_pShaderTexture = new cShaderTexture();
	m_pShaderTexture->Initialize("Shader/ShaderPT.hlsl", shaderResource);

}

void cImage::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	m_pShaderTexture->Update(world, view, proj);
}

void cImage::Render()
{
	UINT stride = sizeof(ST__VERTEX_PT);
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

	m_pShaderTexture->Render(m_indicesSize);
}

void cImage::SetFileAmount(float value)
{
	ST__VERTEX_PT* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ST__VERTEX_PT* verticesPtr;
	HRESULT result;
	
	
	
	
	vertices = new ST__VERTEX_PT[m_vertexSize];
	
	vertices[0].P = m_vecVertex[0].P;  // Top left.
	vertices[0].T = m_vecVertex[0].T;
	
	vertices[1].P = m_vecVertex[1].P;  // Top left.
	vertices[1].T = m_vecVertex[1].T;
	
	vertices[2].P = m_vecVertex[2].P;  // Top left.
	vertices[2].T = D3DXVECTOR2(value, m_vecVertex[2].T.y);
	
	vertices[3].P = m_vecVertex[3].P;  // Top left.
	vertices[3].T = D3DXVECTOR2(value, m_vecVertex[3].T.y);

	result = Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_pVertexBuffer,		
		0,						
		D3D11_MAP_WRITE_DISCARD,
		0,						
		&mappedResource			
	);

	if (FAILED(result))
	{
	}

	verticesPtr = (ST__VERTEX_PT*)mappedResource.pData;
	
	memcpy(verticesPtr, (void*)vertices, (sizeof(ST__VERTEX_PT) * m_vertexSize));
	
	Graphic::GetInstance().GetDeviceContext()->Unmap(m_pVertexBuffer, 0);
	
	delete[] vertices;
	vertices = 0;
}

bool cImage::RayPicking(cRay ray)
{
	D3DXVECTOR3				m_vPickPos;

	for (size_t i = 0; i < m_indicesSize; i+= 3)
	{
		if ( ray.RayPickingTrangle(
				m_vecVertex[m_vecIndices[i]].P,
				m_vecVertex[m_vecIndices[i + 1]].P,
				m_vecVertex[m_vecIndices[i + 2]].P,
				m_vPickPos) )
		{
		}
	}
	return false;
}

bool cImage::RayPickingRect(POINT p)
{
	if (PtInRect(&m_rect, p))
	{
	}
	return false;
}
