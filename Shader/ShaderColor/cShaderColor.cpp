#include "stdafx.h"
#include "Shader/ShaderColor/cShaderColor.h"

cShaderColor::cShaderColor()
{
}

cShaderColor::~cShaderColor()
{
}

void cShaderColor::Initialize(const std::string & shaderFile)
{
	cShaderBase::Initialize(shaderFile);
}

void cShaderColor::Render(UINT drawCount)
{
	cShaderBase::Render(drawCount);
}

void cShaderColor::InitializeInputLayer(ID3DBlob * m_VS_Blob)
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
