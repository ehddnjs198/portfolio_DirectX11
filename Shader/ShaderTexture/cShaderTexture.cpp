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
	);//IA �ܰ迡�� ������ 10���̴� �ϸ� 10�� ȣ���Ѵ�.
	//���� �������� ȣ���� �þ��, ���� ȣ��Ǹ� ����������,
	//GPU�� ���� ����̴�, �ھ ���� ������ Ŀ�� �����ϴ�.

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_lp_VS_Buffer
		//��� ���Ժ��� HLSL�ڵ忡�� buffer�� ����Ҷ� �Է��ߴ� ���� ��ȣ.
		//��� ���۸� �����ų�, 3�� �ڵ����� 0, 1, 2,
		//���� �ֱ�.
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
			input_desc,		//desc ����
			2,				//� �ΰ�
			m_VS_Blob->GetBufferPointer(),
			m_VS_Blob->GetBufferSize(),
			&m_pInputLayer	//�ʱ�ȭ �� ����
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
		//U ��ǥ�� �þ���� � �۾��� �Ұǰ�.
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		//V ��ǥ�� �þ���� � �۾��� �Ұǰ�.
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		//������ ��ǥ�� ���Ѱ�, ���󵵵ȴ�.
		desc.BorderColor[0] = 0; //R
		desc.BorderColor[1] = 0; //G
		desc.BorderColor[2] = 0; //B
		desc.BorderColor[3] = 0; //A
		//�ܰ��� / ������� ���� ������ ĥ�Ҷ�
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		//������ ���� �����͸� ���ϴ� ����� ���ϴ� �÷���
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//�̹����� ���, Ȯ��, �Ӹ��� �Ͼ���� ������.
		desc.MaxAnisotropy = 16;
		//���漺 ���͸�, ���� �ȴ�, 3D�Ҷ� �̹����� �߶����� ȸ���Ȱ�쳪,
		//�ĸ鿡 ���� �̹����� ����ϰ� �Ұ��ΰ��� ���� ���
		desc.MaxLOD = std::numeric_limits<float>::max();
		desc.MinLOD = std::numeric_limits<float>::min();
		//LOD = Level of Detail �Ÿ��� ���� ��� �����ϰų� �հ͵��� �� �Ⱥ��̰ų� ���� �����ϴ�.
		desc.MipLODBias = 0.0f;
		//�Ӹʿ� ���� ��ﶧ ����, �ε��� �����ִ� �߰��Ǵ� �ɼ� ������.

		auto hr = Graphic::GetInstance().GetDevice()->CreateSamplerState
		(
			&desc, &m_lpSamplerState
		);
		assert(SUCCEEDED(hr));
	}
}
