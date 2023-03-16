#include "stdafx.h"
#include "Shader/ShaderSkinned/cShaderSkinned.h"


cShaderSkinned::cShaderSkinned()
{
}

cShaderSkinned::~cShaderSkinned()
{
}

void cShaderSkinned::Initialize(const std::string & shaderFile, const std::string & shaderResource)
{
	cShaderBase::Initialize(shaderFile);

	{//Create constant Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(ST_BONETransform_DATA);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		//CPUAccessFlags 시피유가 접근 했을때 뭘 해야되는지 나타내는 플래그.

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, nullptr, &m_BoneTransform_Buffer
		);

		assert(SUCCEEDED(hr));
	}

	InitializeShaderResource(shaderResource);
}
void cShaderSkinned::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{

	//for (unsigned int i = 0; i < m_BoneInfo->size(); i++) {
	//	//char Name[128];
	//	//memset(Name, 0, sizeof(Name));
	//	//SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
	//
	//	D3DXMATRIX mat;
	//
	//	mat._11 = m_BoneInfo->at(i).FinalTransformation.a1;
	//	mat._12 = m_BoneInfo->at(i).FinalTransformation.a2;
	//	mat._13 = m_BoneInfo->at(i).FinalTransformation.a3;
	//	mat._14 = m_BoneInfo->at(i).FinalTransformation.a4;
	//
	//	mat._21 = m_BoneInfo->at(i).FinalTransformation.b1;
	//	mat._22 = m_BoneInfo->at(i).FinalTransformation.b2;
	//	mat._23 = m_BoneInfo->at(i).FinalTransformation.b3;
	//	mat._24 = m_BoneInfo->at(i).FinalTransformation.b4;
	//
	//	mat._31 = m_BoneInfo->at(i).FinalTransformation.c1;
	//	mat._32 = m_BoneInfo->at(i).FinalTransformation.c2;
	//	mat._33 = m_BoneInfo->at(i).FinalTransformation.c3;
	//	mat._34 = m_BoneInfo->at(i).FinalTransformation.c4;
	//
	//	mat._41 = m_BoneInfo->at(i).FinalTransformation.d1;
	//	mat._42 = m_BoneInfo->at(i).FinalTransformation.d2;
	//	mat._43 = m_BoneInfo->at(i).FinalTransformation.d3;
	//	mat._44 = m_BoneInfo->at(i).FinalTransformation.d4;
	//
	//	m_stTransform_Data.bones[i] = mat;
	//}

	cShaderBase::Update(world, view, proj);
}
void cShaderSkinned::Render(UINT drawCount)
{
	Graphic::GetInstance().GetDeviceContext()->IASetInputLayout(m_pInputLayer);

	Graphic::GetInstance().GetDeviceContext()->VSSetShader
	(
		m_Vertex_Shader, nullptr, 0
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_lp_VS_Buffer
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		1, 1, &m_BoneTransform_Buffer
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
void cShaderSkinned::SkinnedUpdate(std::vector<BoneInfo>* BoneInfo, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	for (unsigned int i = 0; i < BoneInfo->size(); i++) {

		D3DXMATRIX mat;

		mat._11 = BoneInfo->at(i).FinalTransformation.a1;
		mat._12 = BoneInfo->at(i).FinalTransformation.a2;
		mat._13 = BoneInfo->at(i).FinalTransformation.a3;
		mat._14 = BoneInfo->at(i).FinalTransformation.a4;

		mat._21 = BoneInfo->at(i).FinalTransformation.b1;
		mat._22 = BoneInfo->at(i).FinalTransformation.b2;
		mat._23 = BoneInfo->at(i).FinalTransformation.b3;
		mat._24 = BoneInfo->at(i).FinalTransformation.b4;

		mat._31 = BoneInfo->at(i).FinalTransformation.c1;
		mat._32 = BoneInfo->at(i).FinalTransformation.c2;
		mat._33 = BoneInfo->at(i).FinalTransformation.c3;
		mat._34 = BoneInfo->at(i).FinalTransformation.c4;

		mat._41 = BoneInfo->at(i).FinalTransformation.d1;
		mat._42 = BoneInfo->at(i).FinalTransformation.d2;
		mat._43 = (BoneInfo->at(i).FinalTransformation.d3);
		mat._44 = BoneInfo->at(i).FinalTransformation.d4;

		m_BoneData.bones[i] = objectMatrix;
	}

	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	//MAPPED 짝을 맞춰준다 / 맵핑시킨다.
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_BoneTransform_Buffer,				//어떤 리소스를 갱신 할것인가.
		0,							//서브 리소스 몰라도됨
		D3D11_MAP_WRITE_DISCARD,	//맵 타입 -> 쓰고 파괴
		0,							//맵 플러그인 0 안씀
		&mapped_subresource			//리소스.
	);

	memcpy(
		mapped_subresource.pData,	//목적 파일, 복사본
		&m_BoneData,				//원본파일
		sizeof(ST_BONETransform_DATA));	//크기.

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_BoneTransform_Buffer, 0
	);


	cShaderBase::Update(world, view, proj);
}
void cShaderSkinned::InitializeInputLayer(ID3DBlob * m_VS_Blob)
{
	//InputLayer
	{
		D3D11_INPUT_ELEMENT_DESC input_desc[]
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,0, D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,12, D3D11_INPUT_PER_VERTEX_DATA},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0,24, D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT,
			0,32, D3D11_INPUT_PER_VERTEX_DATA},
			{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,48, D3D11_INPUT_PER_VERTEX_DATA},
		};

		auto hr = Graphic::GetInstance().GetDevice()->CreateInputLayout
		(
			input_desc,		//desc 정보
			5,				//몇개 인가
			m_VS_Blob->GetBufferPointer(),
			m_VS_Blob->GetBufferSize(),
			&m_pInputLayer	//초기화 할 정보
		);

		assert(SUCCEEDED(hr));
	}
}
void cShaderSkinned::InitializeShaderResource(const std::string & textureFile)
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
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 0; //R
		desc.BorderColor[1] = 0; //G
		desc.BorderColor[2] = 0; //B
		desc.BorderColor[3] = 0; //A
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.MaxAnisotropy = 16;
		desc.MaxLOD = std::numeric_limits<float>::max();
		desc.MinLOD = std::numeric_limits<float>::min();
		desc.MipLODBias = 0.0f;

		auto hr = Graphic::GetInstance().GetDevice()->CreateSamplerState
		(
			&desc, &m_lpSamplerState
		);
		assert(SUCCEEDED(hr));
	}
}
