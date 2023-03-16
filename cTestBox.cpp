#include "stdafx.h"
#include "cTestBox.h"
#include "Graphic/Graphic.h"

cTestBox::cTestBox(D3DXCOLOR color)
{

	m_pVertexs = new Vertex[8];
	//0
	m_pVertexs[0].m_VecPosition = D3DXVECTOR3(-50.0f, -50.0f, -50.0f);
	m_pVertexs[0].m_Color = color;
	//1
	m_pVertexs[1].m_VecPosition = D3DXVECTOR3(-50.0f, 50.0f, -50.0f);
	m_pVertexs[1].m_Color = color;
	//2
	m_pVertexs[2].m_VecPosition = D3DXVECTOR3(50.0f, -50.0f, -50.0f);
	m_pVertexs[2].m_Color = color;
	//3
	m_pVertexs[3].m_VecPosition = D3DXVECTOR3(50.0f, 50.0f, -50.0f);
	m_pVertexs[3].m_Color = color;

	//0
	m_pVertexs[4].m_VecPosition = D3DXVECTOR3(-50.0f, -50.0f, 50.0f);
	m_pVertexs[4].m_Color = color;
	//1
	m_pVertexs[5].m_VecPosition = D3DXVECTOR3(-50.0f, 50.0f, 50.0f);
	m_pVertexs[5].m_Color = color;
	//2
	m_pVertexs[6].m_VecPosition = D3DXVECTOR3(50.0f, -50.0f, 50.0f);
	m_pVertexs[6].m_Color = color;
	//3
	m_pVertexs[7].m_VecPosition = D3DXVECTOR3(50.0f, 50.0f, 50.0f);
	m_pVertexs[7].m_Color = color;


	{//Vertex Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		//��� ������ ����� ���ΰ�?  //�Һ�
		//��ǥ������ 4������ �ִ�. 
		//GPU , CPU�� �аų� �������� �����ϴ� �÷���.
		//����Ʈ�� GPU�� �а� ���� ������
		//IMMUTABLE = GPU�� �����͸� ������ �ִ�. �� �ΰ��� GPU�� ���ٰ���, 
		//CPU�� ���� �Ұ����ϴ�.
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//��� ������������ �����̳�.
		desc.ByteWidth = sizeof(Vertex) * 8;//���ý��� ���� ũ�⸦ ��ŭ �Ҳ���.
		//�������� ���ý� ������ ��� ����� ��� ���������� ���� ������ ����.

		D3D11_SUBRESOURCE_DATA sub_data;
		//CPU�����͸� GPU�� ���� �� �� �ִ�.
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));

		//���� ����.
		sub_data.pSysMem = m_pVertexs;

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc,	//���ۿ� ���� ����.
			&sub_data, //�ʱ�ȭ ������.
			&m_pVertexBuffer //���ؽ� ����.
		);

		assert(SUCCEEDED(hr));
	}

	//1		3			5		7
	//
	//0		2			4		6

	{//Index Data

		m_idices = new UINT[36]
		{
			//0, 1, 2 , 2, 1, 3
			//3, 1, 0, 3, 0, 2
			//�޸�.
			0,1,
			1,3,
			3,2,
			2,0,

			4,5,
			5,7,
			7,6,
			6,4,

			1,5,
			3,7,
			0,4,
			2,6,

		};



		//������ ���� ������ �� �� �ߺ��Ǵ°��� ���ְ� ����ϱ� �����̴�.
		   //m_idices = new UINT[6]
		   //{
		   //	//0, 1, 2 , 2, 1, 3
		   //	//3, 1, 0, 3, 0, 2

		   //	//�޸�.
		   //	0,1,2,	2,1,3	,
		   //	////�ո�
		   //	//4,5,6,	6,5,7	,
		   //	////4,6,7,	4,7,5	,
		   //	////����
		   //	//0,1,5,	5,4,0,
		   //	////������
		   //	//2,3,7,	7,6,2,

		   //	////�ظ�
		   //	//0,4,6,	6,2,0,
		   //	////����
		   //	//1,5,7,	7,3,1
		   //};
	}

	{//Index Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(UINT) * 36;
		//
		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = m_idices;
		//
		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, &sub_data, &m_Idices_Buffer
		);

		assert(SUCCEEDED(hr));
	}

	{//Vertex Shader
		//A �ƽ�Ű
		auto hr = D3DX11CompileFromFileA
		("Shader/Color.hlsl", //�ش� ����
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
			input_desc,		//desc ����
			2,				//� �ΰ�
			m_VS_Blob->GetBufferPointer(),
			m_VS_Blob->GetBufferSize(),
			&m_pInputLayer	//�ʱ�ȭ �� ����
		);

		assert(SUCCEEDED(hr));
	}

	{//Pixel Shader
		auto hr = D3DX11CompileFromFileA
		("Shader/Color.hlsl", //�ش� ����
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

	{//��� �ʱ�ȭ

	}

	{//Create constant Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		/*
		D3D11_USAGE_DYNAMIC �������� ���⸸, �������� �б⸸ �����ϴ�.
		*/
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(TRANSFORM_DATA);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, nullptr, &m_gpu_Buffer
		);

		assert(SUCCEEDED(hr));
	}

	{//Create Rasterizer State
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;

		auto hr = Graphic::GetInstance().GetDevice()->CreateRasterizerState
		(
			&desc, &m_Rasterizer_State
		);
		assert(SUCCEEDED(hr));
	}

	
}


cTestBox::~cTestBox()
{
	SAFE_RELEASE(m_Rasterizer_State);

	SAFE_RELEASE(m_Pixel_Shader);
	SAFE_RELEASE(m_PS_Blob);
	//
	SAFE_RELEASE(m_pInputLayer);
	//
	SAFE_RELEASE(m_Vertex_Shader);
	SAFE_RELEASE(m_VS_Blob);
	//IA �ε��� ���� ������ ������ �����ϴ� ���� ����
	//Index BUffer�� IA�� ����.
	SAFE_RELEASE(m_Idices_Buffer);
	SAFE_DELETE_ARRAY(m_idices);

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_DELETE_ARRAY(m_pVertexs);
}

void cTestBox::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{

	D3DXMatrixTranspose(&m_cup_Buffer.m_Mat_Word, &world);
	D3DXMatrixTranspose(&m_cup_Buffer.m_Mat_View, &view);
	D3DXMatrixTranspose(&m_cup_Buffer.m_Mat_Projection, &proj);

	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	//MAPPED ¦�� �����ش� / ���ν�Ų��.
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_gpu_Buffer,				//� ���ҽ��� ���� �Ұ��ΰ�.
		0,							//���� ���ҽ� ���󵵵�
		D3D11_MAP_WRITE_DISCARD,	//�� Ÿ�� -> ���� �ı�
		0,							//�� �÷����� 0 �Ⱦ�
		&mapped_subresource			//���ҽ�.
	);

	memcpy(
		mapped_subresource.pData,	//���� ����, ���纻
		&m_cup_Buffer,				//��������
		sizeof(TRANSFORM_DATA));	//ũ��.

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_gpu_Buffer, 0
	);
}

void cTestBox::Render()
{
	UINT stride = sizeof(Vertex);
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
	//
	Graphic::GetInstance().GetDeviceContext()->IASetInputLayout(m_pInputLayer);

	Graphic::GetInstance().GetDeviceContext()->IASetPrimitiveTopology
	(
		//�ﰢ���� �׸��� ���θ� ä����. ��������� �⺻ ���� IA�ܰ�.
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetShader
	(
		m_Vertex_Shader, nullptr, 0
	);//IA �ܰ迡�� ������ 10���̴� �ϸ� 10�� ȣ���Ѵ�.
	//���� �������� ȣ���� �þ��, ���� ȣ��Ǹ� ����������,
	//GPU�� ���� ����̴�, �ھ ���� ������ Ŀ�� �����ϴ�.

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_gpu_Buffer
		//��� ���Ժ��� HLSL�ڵ忡�� buffer�� ����Ҷ� �Է��ߴ� ���� ��ȣ.
		//��� ���۸� �����ų�, 3�� �ڵ����� 0, 1, 2,
		//���� �ֱ�.
	);

	Graphic::GetInstance().GetDeviceContext()->RSSetState
	(m_Rasterizer_State);

	//////////
	Graphic::GetInstance().GetDeviceContext()->PSSetShader
	(
		m_Pixel_Shader, nullptr, 0
	);

	Graphic::GetInstance().GetDeviceContext()->DrawIndexed(36, 0, 0);
}