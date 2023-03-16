#include "stdafx.h"
#include "cUIImage.h"
#include "Graphic/Graphic.h"
#include "Shader/ShaderTexture/cShaderTexture.h"
#include "Ray/cRay.h"


cUIImage::cUIImage()
{
}


cUIImage::~cUIImage()
{
	SAFE_DELETE(m_pShaderTexture);
	SAFE_RELEASE(m_Idices_Buffer);
	SAFE_RELEASE(m_pVertexBuffer);
}

void cUIImage::Initialize(const std::string & shaderResource, D3DXVECTOR3 pos, int width, int height)
{
	float left, right, top, bottom;
	
	left = ((Setting::GetInstance().GetWindowWidth() / 2) * -1) + pos.x;
	right = left + (float)width;
	top = (Setting::GetInstance().GetWindowHeight() / 2) - pos.y;
	bottom = top - height;
	

	D3DXVECTOR3 vec[4] =
	{
		D3DXVECTOR3(left, bottom, 0.0f),	
		D3DXVECTOR3(left, top, 0.0f),	//1
		D3DXVECTOR3(right, bottom, 0.0f),	
		D3DXVECTOR3(right, top, 0.0f),	//3
	};

	m_Rect =
	{
		static_cast<LONG>(pos.x),
		static_cast<LONG>(pos.y),

	
		static_cast<LONG>(abs(pos.x * 2) + width),
		static_cast<LONG>(abs(pos.y * 2) + height),
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
			&desc,	
			&sub_data, 
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

	D3DXMatrixIdentity(&m_matWorld);

}

void cUIImage::Update(D3DXMATRIX view, D3DXMATRIX proj)
{
	cUIBase::Update(view, proj);
	m_pShaderTexture->Update(m_matWorld, view, proj);
}

void cUIImage::Render()
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

	cUIBase::Render();
}


cUIButtonDelegate * cUIImage::GetDelegate() const
{
	return m_pDelegate;
}


