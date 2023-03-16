#include "stdafx.h"
#include "Grid/cGrid.h"
#include "Graphic/Graphic.h"

cGrid::cGrid(const int &sizeX, const int &sizeY)
{
	//정점을 구한다

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
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//어느 스테이지에서 쓸것이냐.
		desc.ByteWidth = sizeof(Vertex) * vertexSize;//버택스의 버퍼 크기를 얼만큼 할꺼냐.

		D3D11_SUBRESOURCE_DATA sub_data;
		//CPU데이터를 GPU에 연결 할 수 있다.
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));

		//버퍼 연결.
		sub_data.pSysMem = m_pVertexs;

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc,	//버퍼에 대한 설명서.
			&sub_data, //초기화 데이터.
			&m_pVertexBuffer //버텍스 버퍼.
		);

		assert(SUCCEEDED(hr));
	}

	//1		3
	//
	//0		2

	{//Index Data

		IdicesSize = ((sizeX + 1) + (sizeY + 1)) * 2;

		//4 x 4일시
		m_idices = new UINT[IdicesSize];

		int index = 0;

		//가로
		for (int i = 0; i <= sizeX; i++)
		{
			m_idices[index] = i * (sizeY + 1);
			m_idices[index + 1] = m_idices[index] + sizeY;
			//2
			index += 2;
		}

		//세로
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
		//A 아스키
		auto hr = D3DX11CompileFromFileA
		("Shader/Color.hlsl", //해당 파일
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

	{//Pixel Shader
		auto hr = D3DX11CompileFromFileA
		("Shader/Color.hlsl", //해당 파일
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

	{//Create constant Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		/*
		D3D11_USAGE_DYNAMIC 시피유는 쓰기만, 쥐피유는 읽기만 가능하다.
		*/
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(TRANSFORM_DATA);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//CPUAccessFlags 시피유가 접근 했을때 뭘 해야되는지 나타내는 플래그.

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
	//IA 인덱스 버퍼 정점에 순번을 결정하는 정점 정보
	//Index BUffer은 IA에 들어간다.
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
	//MAPPED 짝을 맞춰준다 / 맵핑시킨다.
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_gpu_Buffer,				//어떤 리소스를 갱신 할것인가.
		0,							//서브 리소스 몰라도됨
		D3D11_MAP_WRITE_DISCARD,	//맵 타입 -> 쓰고 파괴
		0,							//맵 플러그인 0 안씀
		&mapped_subresource			//리소스.
	);

	memcpy(
		mapped_subresource.pData,	//목적 파일, 복사본
		&m_cup_Buffer,				//원본파일
		sizeof(TRANSFORM_DATA));	//크기.

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_gpu_Buffer, 0
	);
}

void cGrid::Render()
{
	//버택스 버퍼에 대한 정보는 일자로 들어가있는 상태.
	//P1/P2/P3 정점 하나의 크기 값이 필요하다.
	UINT stride = sizeof(Vertex);
	//정점 정보중에 어떤 정점부터 사용할 것인지
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
		//삼각형을 그리고 내부를 채워라. 여기까지가 기본 셋팅 IA단계.
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
