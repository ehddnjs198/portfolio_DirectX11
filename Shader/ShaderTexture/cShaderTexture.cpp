#include "stdafx.h"
#include "Shader/ShaderTexture/cShaderTexture.h"

cShaderTexture::cShaderTexture()
{
}

cShaderTexture::~cShaderTexture()
{
}

void cShaderTexture::Initialize(const std::string & shaderFile, const std::string & shaderResource)
{
	cShaderBase::Initialize(shaderFile);
	InitializeShaderResource(shaderResource);
}
void cShaderTexture::Render(UINT drawCount)
{
	Graphic::GetInstance().GetDeviceContext()->IASetInputLayout(m_pInputLayer);

	Graphic::GetInstance().GetDeviceContext()->VSSetShader
	(
		m_Vertex_Shader, nullptr, 0
	);//IA 단계에서 정점이 10개이다 하면 10번 호출한다.
	//정점 갯수마다 호출이 늘어난다, 많이 호출되면 느려지지만,
	//GPU는 병렬 방식이다, 코어가 많기 떄문에 커버 가능하다.

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_lp_VS_Buffer
		//몇번 슬롯부터 HLSL코드에서 buffer를 등록할때 입력했던 버퍼 번호.
		//몇개의 버퍼를 넣을거냐, 3을 자동으로 0, 1, 2,
		//버퍼 넣기.
	);

	Graphic::GetInstance().GetDeviceContext()->RSSetState
	(m_lp_Rasterizer_State);

	//////////
	Graphic::GetInstance().GetDeviceContext()->PSSetShader
	(
		m_Pixel_Shader, nullptr, 0
	);

	Graphic::GetInstance().GetDeviceContext()->PSSetShaderResources
	(
		0, 1,
		&m_lpShaderResource
	);

	Graphic::GetInstance().GetDeviceContext()->PSSetSamplers(0, 1, &m_lpSamplerState);


	Graphic::GetInstance().GetDeviceContext()->DrawIndexed(drawCount, 0, 0);
}
void cShaderTexture::InitializeInputLayer(ID3DBlob * m_VS_Blob)
{
	{//InputLayer
		D3D11_INPUT_ELEMENT_DESC input_desc[]
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,0, D3D11_INPUT_PER_VERTEX_DATA},
			{"TEXTURECOOD", 0, DXGI_FORMAT_R32G32_FLOAT,
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
void cShaderTexture::InitializeShaderResource(const std::string & textureFile)
{
	{//Create Shader Resource View
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			Graphic::GetInstance().GetDevice(),
			textureFile.c_str(), //"Image.png",
			nullptr,
			nullptr,
			&m_lpShaderResource,
			nullptr
		);

		assert(SUCCEEDED(hr));
	}

	{//Create Sample State
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		//U 좌표가 늘어났을때 어떤 작업을 할건가.
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		//V 좌표가 늘어났을때 어떤 작업을 할건가.
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		//나머지 좌표에 대한것, 몰라도된다.
		desc.BorderColor[0] = 0; //R
		desc.BorderColor[1] = 0; //G
		desc.BorderColor[2] = 0; //B
		desc.BorderColor[3] = 0; //A
		//외각선 / 빈공간에 대한 색깔을 칠할때
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		//이전과 현재 데이터를 비교하는 방법을 정하는 플래그
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//이미지가 축소, 확대, 밉맵이 일어날때에 보정법.
		desc.MaxAnisotropy = 16;
		//비등방성 필터링, 몰라도 된다, 3D할때 이미지를 발랐을때 회전된경우나,
		//후면에 대해 이미지를 깔끔하게 할것인가에 대한 방법
		desc.MaxLOD = std::numeric_limits<float>::max();
		desc.MinLOD = std::numeric_limits<float>::min();
		//LOD = Level of Detail 거리에 따라서 어떻게 조밀하거나 먼것들은 잘 안보이거나 조정 가능하다.
		desc.MipLODBias = 0.0f;
		//밉맵에 대해 배울때 설명, 인덱스 번들있다 추가되는 옵셋 정보들.

		auto hr = Graphic::GetInstance().GetDevice()->CreateSamplerState
		(
			&desc, &m_lpSamplerState
		);
		assert(SUCCEEDED(hr));
	}
}
