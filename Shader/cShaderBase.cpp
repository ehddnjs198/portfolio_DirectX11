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
		//A �ƽ�Ű
		auto hr = D3DX11CompileFromFileA
		(shaderFile.c_str(), //�ش� ����
			nullptr,//��ũ�� �Ĺݿ� �Ұ���. ���̴����� �����ι��� �ѱ�� �ִ�.
			nullptr,//Include ������ �����ϴ�.
			"VS",
			"vs_5_0", //1, 2, 3, 4, 5 ���̴� ���� �ְ� ������ �����̴�.
			0,
			0,
			nullptr, //������ ����, ���󵵵�
			&m_VS_Blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = Graphic::GetInstance().GetDevice()->CreateVertexShader
		(
			m_VS_Blob->GetBufferPointer(), //���̴� ����Ʈ �ڵ�, Blob ����
			//�������� ���������� �ּҸ� �ѱ� �� �ִ�.
			m_VS_Blob->GetBufferSize(), //����� ũ��,
			nullptr,
			&m_Vertex_Shader
		);

		assert(SUCCEEDED(hr));
	}

	{//Pixel Shader
		auto hr = D3DX11CompileFromFileA
		(shaderFile.c_str(), //�ش� ����
			nullptr,//��ũ�� �Ĺݿ� �Ұ���. ���̴����� �����ι��� �ѱ�� �ִ�.
			nullptr,//Include ������ �����ϴ�.
			"PS",
			"ps_5_0", //1, 2, 3, 4, 5 ���̴� ���� �ְ� ������ �����̴�.
			0,
			0,
			nullptr, //������ ����, ���󵵵�
			&m_PS_Blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = Graphic::GetInstance().GetDevice()->CreatePixelShader
		(
			m_PS_Blob->GetBufferPointer(), //���̴� ����Ʈ �ڵ�, Blob ����
			//�������� ���������� �ּҸ� �ѱ� �� �ִ�.
			m_PS_Blob->GetBufferSize(), //����� ũ��,
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
		D3D11_USAGE_DYNAMIC �������� ���⸸, �������� �б⸸ �����ϴ�.
		*/
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(ST__VSTransform_Data);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//CPUAccessFlags �������� ���� ������ �� �ؾߵǴ��� ��Ÿ���� �÷���.
	
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
	//MAPPED ¦�� �����ش� / ���ν�Ų��.
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_lp_VS_Buffer,				//� ���ҽ��� ���� �Ұ��ΰ�.
		0,							//���� ���ҽ� ���󵵵�
		D3D11_MAP_WRITE_DISCARD,	//�� Ÿ�� -> ���� �ı�
		0,							//�� �÷����� 0 �Ⱦ�
		&mapped_subresource			//���ҽ�.
	);

	memcpy(
		mapped_subresource.pData,	//���� ����, ���纻
		&m_st_VS_Data,				//��������
		sizeof(ST__VSTransform_Data));	//ũ��.
	
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
	
	Graphic::GetInstance().GetDeviceContext()->DrawIndexed(drawCount, 0, 0);
}
