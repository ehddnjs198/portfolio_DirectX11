#include "stdafx.h"
#include "cShaderFire.h"

cShaderFire::cShaderFire()
{
}

cShaderFire::~cShaderFire()
{
}

void cShaderFire::Initialize(const std::string &shaderFile)
{
	cShaderBase::Initialize(shaderFile);

	{//Create Shader Resource View
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			Graphic::GetInstance().GetDevice(),
			"Resources/fire01.dds",//textureFile.c_str(), //"Image.png",
			nullptr,
			nullptr,
			&m_ResourceView[0],
			nullptr
		);

		assert(SUCCEEDED(hr));
	}
	{//Create Shader Resource View
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			Graphic::GetInstance().GetDevice(),
			"Resources/noise01.dds",//textureFile.c_str(), //"Image.png",
			nullptr,
			nullptr,
			&m_ResourceView[1],
			nullptr
		);

		assert(SUCCEEDED(hr));
	}
	{//Create Shader Resource View
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			Graphic::GetInstance().GetDevice(),
			"Resources/alpha01.dds",//textureFile.c_str(), //"Image.png",
			nullptr,
			nullptr,
			&m_ResourceView[2],
			nullptr
		);

		assert(SUCCEEDED(hr));
	}

	// 버텍스 쉐이더에있는 동적 노이즈 상수 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC noiseBufferDesc;
	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 노이즈 버퍼 포인터를 만듭니다.
	auto result = Graphic::GetInstance().GetDevice()->CreateBuffer(&noiseBufferDesc, NULL, &m_noiseBuffer);
	assert(SUCCEEDED(result));

	// 텍스처 샘플러 상태 설명을 만듭니다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	result = Graphic::GetInstance().GetDevice()->CreateSamplerState(&samplerDesc, &m_sampleState);
	assert(SUCCEEDED(result));

	// 클램프 샘플러에 대한 두 번째 텍스처 샘플러 상태 설명을 만듭니다.
	D3D11_SAMPLER_DESC samplerDesc2;
	samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.MipLODBias = 0.0f;
	samplerDesc2.MaxAnisotropy = 1;
	samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc2.BorderColor[0] = 0;
	samplerDesc2.BorderColor[1] = 0;
	samplerDesc2.BorderColor[2] = 0;
	samplerDesc2.BorderColor[3] = 0;
	samplerDesc2.MinLOD = 0;
	samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	result = Graphic::GetInstance().GetDevice()->CreateSamplerState(&samplerDesc2, &m_sampleState2);
	assert(SUCCEEDED(result));

	// 픽셀 쉐이더에있는 동적 왜곡 상수 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC distortionBufferDesc;
	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 픽셀 쉐이더 상수 버퍼에 액세스 할 수 있도록 왜곡 버퍼 포인터를 만듭니다.
	result = Graphic::GetInstance().GetDevice()->CreateBuffer(
		&distortionBufferDesc,NULL, &m_distortionBuffer);

	assert(SUCCEEDED(result));

}

void cShaderFire::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	cShaderBase::Update(world, view, proj);

	// 프레임 시간 카운터를 증가시킵니다.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	m_noiseData.frameTime = frameTime;
	m_noiseData.scrollSpeeds = scrollSpeeds;
	m_noiseData.scales = scales;
	m_noiseData.padding = 0.0f;

	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	//MAPPED 짝을 맞춰준다 / 맵핑시킨다.
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_noiseBuffer,				//어떤 리소스를 갱신 할것인가.
		0,							//서브 리소스 몰라도됨
		D3D11_MAP_WRITE_DISCARD,	//맵 타입 -> 쓰고 파괴
		0,							//맵 플러그인 0 안씀
		&mapped_subresource			//리소스.
	);

	memcpy(
		mapped_subresource.pData,	//목적 파일, 복사본
		&m_noiseData,				//원본파일
		sizeof(NoiseBufferType));	//크기.

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_noiseBuffer, 0
	);
	////////////////////
	{
		D3DXVECTOR2 distortion1 = D3DXVECTOR2(0.1f, 0.2f);
		D3DXVECTOR2 distortion2 = D3DXVECTOR2(0.1f, 0.3f);
		D3DXVECTOR2 distortion3 = D3DXVECTOR2(0.1f, 0.1f);
		// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
		float distortionScale = 0.8f;
		float distortionBias = 0.5f;
		// 왜곡 상수 버퍼에 데이터를 복사합니다.
		m_distortionData.distortion1 = distortion1;
		m_distortionData.distortion2 = distortion2;
		m_distortionData.distortion3 = distortion3;
		m_distortionData.distortionScale = distortionScale;
		m_distortionData.distortionBias = distortionBias;

		D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
		//MAPPED 짝을 맞춰준다 / 맵핑시킨다.
		Graphic::GetInstance().GetDeviceContext()->Map
		(
			m_distortionBuffer,			
			0,							
			D3D11_MAP_WRITE_DISCARD,	
			0,							
			&mapped_subresource			
		);

		memcpy(
			mapped_subresource.pData,	
			&m_distortionData,			
			sizeof(DistortionBufferType));	

		Graphic::GetInstance().GetDeviceContext()->Unmap
		(
			m_distortionBuffer, 0
		);
	}
}

void cShaderFire::Render(UINT drawCount)
{

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_noiseBuffer);
	Graphic::GetInstance().GetDeviceContext()->PSSetShaderResources(0, 3, m_ResourceView);
	Graphic::GetInstance().GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_distortionBuffer);

	Graphic::GetInstance().GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);
	Graphic::GetInstance().GetDeviceContext()->PSSetSamplers(1, 1, &m_sampleState2);

	cShaderBase::Render(drawCount);
}

void cShaderFire::InitializeInputLayer(ID3DBlob * m_VS_Blob)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	auto hr = Graphic::GetInstance().GetDevice()->CreateInputLayout
	(
		polygonLayout,	//desc 정보
		2,				//몇개 인가
		m_VS_Blob->GetBufferPointer(),
		m_VS_Blob->GetBufferSize(),
		&m_pInputLayer	//초기화 할 정보
	);

	assert(SUCCEEDED(hr));
}

