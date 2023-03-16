#include "stdafx.h"
#include "Shader/cShaderBase.h"

cShaderBase::cShaderBase()
{
}


cShaderBase::~cShaderBase()
{
	SAFE_RELEASE(m_lp_Rasterizer_State);
	SAFE_RELEASE(m_lp_VS_Buffer);
	SAFE_RELEASE(m_pInputLayer);
	SAFE_RELEASE(m_Pixel_Shader);
	SAFE_RELEASE(m_Vertex_Shader);
}

void cShaderBase::Initialize(const std::string &shaderFile)
{
	ID3DBlob* m_VS_Blob = nullptr;
	ID3DBlob* m_PS_Blob = nullptr;

	{//Vertex Shader
		//A 아스키
		auto hr = D3DX11CompileFromFileA
		(shaderFile.c_str(), //해당 파일
			nullptr,//매크로 후반에 할것임. 쉐이더에서 디파인문을 넘길수 있다.
			nullptr,//Include 수용이 가능하다.
			"VS",
			"vs_5_0", //1, 2, 3, 4, 5 쉐이더 버젼 최고 버젼을 쓸것이다.
			0,
			0,
			nullptr, //스레드 펌프, 몰라도됨
			&m_VS_Blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = Graphic::GetInstance().GetDevice()->CreateVertexShader
		(
			m_VS_Blob->GetBufferPointer(), //쉐이더 바이트 코드, Blob 정보
			//바이터의 시작정보인 주소를 넘길 수 있다.
			m_VS_Blob->GetBufferSize(), //블롭의 크기,
			nullptr,
			&m_Vertex_Shader
		);

		assert(SUCCEEDED(hr));
	}

	{//Pixel Shader
		auto hr = D3DX11CompileFromFileA
		(shaderFile.c_str(), //해당 파일
			nullptr,//매크로 후반에 할것임. 쉐이더에서 디파인문을 넘길수 있다.
			nullptr,//Include 수용이 가능하다.
			"PS",
			"ps_5_0", //1, 2, 3, 4, 5 쉐이더 버젼 최고 버젼을 쓸것이다.
			0,
			0,
			nullptr, //스레드 펌프, 몰라도됨
			&m_PS_Blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = Graphic::GetInstance().GetDevice()->CreatePixelShader
		(
			m_PS_Blob->GetBufferPointer(), //쉐이더 바이트 코드, Blob 정보
			//바이터의 시작정보인 주소를 넘길 수 있다.
			m_PS_Blob->GetBufferSize(), //블롭의 크기,
			nullptr,
			&m_Pixel_Shader
		);

		assert(SUCCEEDED(hr));
	}

	InitializeInputLayer(m_VS_Blob);

	SAFE_RELEASE(m_PS_Blob);
	SAFE_RELEASE(m_VS_Blob);

	{//Create Rasterizer State
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;
	
		auto hr = Graphic::GetInstance().GetDevice()->CreateRasterizerState
		(
			&desc, &m_lp_Rasterizer_State
		);
	
		assert(SUCCEEDED(hr));
	}

	{//Create constant Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		/*
		D3D11_USAGE_DYNAMIC 시피유는 쓰기만, 쥐피유는 읽기만 가능하다.
		*/
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(ST__VSTransform_Data);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//CPUAccessFlags 시피유가 접근 했을때 뭘 해야되는지 나타내는 플래그.
	
		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, nullptr, &m_lp_VS_Buffer
		);
	
		assert(SUCCEEDED(hr));
	}

}

void cShaderBase::Initialize(const std::string & shaderFile, const std::string & shaderResource)
{
	Initialize(shaderFile);
}

void cShaderBase::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	

	D3DXMatrixTranspose(&m_st_VS_Data.m_matWorld, &world);
	D3DXMatrixTranspose(&m_st_VS_Data.m_matView, &view);
	D3DXMatrixTranspose(&m_st_VS_Data.m_matProj, &proj);
	
	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	//MAPPED 짝을 맞춰준다 / 맵핑시킨다.
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_lp_VS_Buffer,				//어떤 리소스를 갱신 할것인가.
		0,							//서브 리소스 몰라도됨
		D3D11_MAP_WRITE_DISCARD,	//맵 타입 -> 쓰고 파괴
		0,							//맵 플러그인 0 안씀
		&mapped_subresource			//리소스.
	);

	memcpy(
		mapped_subresource.pData,	//목적 파일, 복사본
		&m_st_VS_Data,				//원본파일
		sizeof(ST__VSTransform_Data));	//크기.
	
	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_lp_VS_Buffer, 0
	);
}

void cShaderBase::Render(UINT drawCount)
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
	
	Graphic::GetInstance().GetDeviceContext()->DrawIndexed(drawCount, 0, 0);
}
