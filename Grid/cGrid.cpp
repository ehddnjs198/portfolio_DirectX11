#include "stdafx.h"
#include "Grid/cGrid.h"
#include "Graphic/Graphic.h"

cGrid::cGrid(const int &sizeX, const int &sizeY)
{
	//������ ���Ѵ�

	vertexSize = (sizeX + 1) * (sizeY + 1);

	m_pVertexs = new Vertex[vertexSize];

	D3DXVECTOR3 xx = D3DXVECTOR3(-sizeX / (float)2, 0, sizeY / (float)2);

	int count = 0;

	for (int i = 0; i < vertexSize; ++i)
	{
		if (count >= sizeY + 1)
		{
			xx = D3DXVECTOR3(-sizeX / (float)2, 0, xx.z);
			xx.z -= 1.f;
			count = 0;
		}

		m_pVertexs[i].m_VecPosition = xx;
		m_pVertexs[i].m_Color = D3DXCOLOR(1, 1, 1, 1);

		xx.x += 1.f;
		count++;
	}

	{//Vertex Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//��� ������������ �����̳�.
		desc.ByteWidth = sizeof(Vertex) * vertexSize;//���ý��� ���� ũ�⸦ ��ŭ �Ҳ���.

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

	//1		3
	//
	//0		2

	{//Index Data

		IdicesSize = ((sizeX + 1) + (sizeY + 1)) * 2;

		//4 x 4�Ͻ�
		m_idices = new UINT[IdicesSize];

		int index = 0;

		//����
		for (int i = 0; i <= sizeX; i++)
		{
			m_idices[index] = i * (sizeY + 1);
			m_idices[index + 1] = m_idices[index] + sizeY;
			//2
			index += 2;
		}

		//����
		for (int i = 0; i <= sizeY; i++)
		{
			m_idices[index] = i;
			m_idices[index + 1] = m_idices[index] + (sizeX * (sizeY + 1));
			index += 2;
		}


		for (int i = 0; i < IdicesSize; i+=2)
		{
		}
	}



	{//Index Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(UINT) * IdicesSize;
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
		//CPUAccessFlags �������� ���� ������ �� �ؾߵǴ��� ��Ÿ���� �÷���.

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


cGrid::~cGrid()
{
	//SAFE_RELEASE(m_Rasterizer_State);

	SAFE_RELEASE(m_gpu_Buffer);

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

void cGrid::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
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

void cGrid::Render()
{
	//���ý� ���ۿ� ���� ������ ���ڷ� ���ִ� ����.
	//P1/P2/P3 ���� �ϳ��� ũ�� ���� �ʿ��ϴ�.
	UINT stride = sizeof(Vertex);
	//���� �����߿� � �������� ����� ������
	UINT offset = 0;

	//Graphic::GetInstance().Begin();

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
		//�ﰢ���� �׸��� ���θ� ä����. ��������� �⺻ ���� IA�ܰ�.
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		//D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetShader
	(
		m_Vertex_Shader, nullptr, 0
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_gpu_Buffer
		
	);

	Graphic::GetInstance().GetDeviceContext()->RSSetState
	(m_Rasterizer_State);

	//////////
	Graphic::GetInstance().GetDeviceContext()->PSSetShader
	(
		m_Pixel_Shader, nullptr, 0
	);

	
	Graphic::GetInstance().GetDeviceContext()->DrawIndexed(IdicesSize, 0, 0);
}
