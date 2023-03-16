#include "stdafx.h"
#include "cShaderColor_0.h"

cShaderColor_0::cShaderColor_0()
	: m_Color(0, 0, 0, 0)
{
}

cShaderColor_0::~cShaderColor_0()
{
	SAFE_RELEASE(m_lp_PS_Color_Buffer);
}

void cShaderColor_0::Initialize(const std::string & shaderFile)
{
	cShaderBase::Initialize(shaderFile);

	{//Create constant Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		/*
		*/
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(D3DXCOLOR);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, nullptr, &m_lp_PS_Color_Buffer
		);

		assert(SUCCEEDED(hr));
	}

}

void cShaderColor_0::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	cShaderBase::Update(world, view, proj);
	
	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_lp_PS_Color_Buffer,	
		0,						
		D3D11_MAP_WRITE_DISCARD,
		0,						
		&mapped_subresource		
	);

	memcpy(
		mapped_subresource.pData,	
		&m_Color,				
		sizeof(D3DXCOLOR));	

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_lp_PS_Color_Buffer, 0
	);

}

void cShaderColor_0::Render(UINT drawCount)
{
	Graphic::GetInstance().GetDeviceContext()->PSSetConstantBuffers
	(
		0, 1, &m_lp_PS_Color_Buffer
	);

	cShaderBase::Render(drawCount);
}

void cShaderColor_0::InitializeInputLayer(ID3DBlob * m_VS_Blob)
{
	{//InputLayer
		D3D11_INPUT_ELEMENT_DESC input_desc[]
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,0, D3D11_INPUT_PER_VERTEX_DATA},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,12, D3D11_INPUT_PER_VERTEX_DATA},
		};

		auto hr = Graphic::GetInstance().GetDevice()->CreateInputLayout
		(
			input_desc,		//desc 정보
			2,				//몇개 인가
			m_VS_Blob->GetBufferPointer(),
			m_VS_Blob->GetBufferSize(),
			&m_pInputLayer	//초기화 할 정보
		);

		assert(SUCCEEDED(hr));
	}
}
