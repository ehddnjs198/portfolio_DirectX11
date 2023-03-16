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
		//어떻게 정보를 사용할 것인가?  //불변
		//대표적으로 4가지가 있다. 
		//GPU , CPU가 읽거나 쓸것인지 결정하는 플래그.
		//디펄트는 GPU가 읽고 쓸수 있으며
		//IMMUTABLE = GPU만 데이터를 읽을수 있다. 이 두가는 GPU만 접근가능, 
		//CPU는 접근 불가능하다.
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//어느 스테이지에서 쓸것이냐.
		desc.ByteWidth = sizeof(Vertex) * 8;//버택스의 버퍼 크기를 얼만큼 할꺼냐.
		//여까지는 버택스 정보를 어떻게 만들고 어떻게 쓸것인지에 대한 껍데기 정보.

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

	//1		3			5		7
	//
	//0		2			4		6

	{//Index Data

		m_idices = new UINT[36]
		{
			//0, 1, 2 , 2, 1, 3
			//3, 1, 0, 3, 0, 2
			//뒷면.
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



		//무수히 많은 정점을 좀 더 중복되는것을 없애고 사용하기 위함이다.
		   //m_idices = new UINT[6]
		   //{
		   //	//0, 1, 2 , 2, 1, 3
		   //	//3, 1, 0, 3, 0, 2

		   //	//뒷면.
		   //	0,1,2,	2,1,3	,
		   //	////앞면
		   //	//4,5,6,	6,5,7	,
		   //	////4,6,7,	4,7,5	,
		   //	////왼쪽
		   //	//0,1,5,	5,4,0,
		   //	////오른쪽
		   //	//2,3,7,	7,6,2,

		   //	////밑면
		   //	//0,4,6,	6,2,0,
		   //	////윗면
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

	{//행렬 초기화

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
	//IA 인덱스 버퍼 정점에 순번을 결정하는 정점 정보
	//Index BUffer은 IA에 들어간다.
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
		//삼각형을 그리고 내부를 채워라. 여기까지가 기본 셋팅 IA단계.
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetShader
	(
		m_Vertex_Shader, nullptr, 0
	);//IA 단계에서 정점이 10개이다 하면 10번 호출한다.
	//정점 갯수마다 호출이 늘어난다, 많이 호출되면 느려지지만,
	//GPU는 병렬 방식이다, 코어가 많기 떄문에 커버 가능하다.

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_gpu_Buffer
		//몇번 슬롯부터 HLSL코드에서 buffer를 등록할때 입력했던 버퍼 번호.
		//몇개의 버퍼를 넣을거냐, 3을 자동으로 0, 1, 2,
		//버퍼 넣기.
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