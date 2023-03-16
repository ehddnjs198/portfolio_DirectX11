#include "stdafx.h"
#include "Graphic/Graphic.h"

Graphic::Graphic()
{

}

Graphic::~Graphic()
{
	SAFE_RELEASE(m_depthStencilBuffer);
	SAFE_RELEASE(m_depthStencilState);
	SAFE_RELEASE(m_depthStencilView);

	SAFE_RELEASE(pRender_Target_View);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pDevice_Context);
	SAFE_RELEASE(pSwap_Chain);
	
}

void Graphic::Initialize()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.RefreshRate.Numerator = 60; // ����
	desc.BufferDesc.RefreshRate.Denominator = 1; // �и�
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//ȭ���� Ȯ��ǰų� ��ҵɋ� �����ϰ� �̰͵� �𸣰ڴٶ�� �˷��ش�.
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.BufferCount = 1;
	desc.SampleDesc.Count = 1;

	desc.SampleDesc.Quality = 0;
	desc.OutputWindow = Setting::GetInstance().GetWindowHandle();
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	std::vector<D3D_FEATURE_LEVEL> feature
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	auto hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		feature.data(),
		feature.size(),
		D3D11_SDK_VERSION,
		&desc,
		&pSwap_Chain,
		&pDevice,
		nullptr,
		&pDevice_Context
	);
	assert(SUCCEEDED(hr) && "ABD");

}

void Graphic::CreateBackBuffer(const UINT & width, const UINT & height)
{
	auto hr = pSwap_Chain->ResizeBuffers
	(
		0,
		width,
		height,
		//�𸣰ڴ�, �̹� �״�� �ִ°� ���ڴ�.
		DXGI_FORMAT_UNKNOWN,
		0
	);

	assert(SUCCEEDED(hr));

	//2)
	ID3D11Texture2D * back_buffer = nullptr;

	hr = pSwap_Chain->GetBuffer
	(
		0,
		__uuidof(ID3D11Texture2D),
		
		reinterpret_cast<void**>(&back_buffer)
	);

	assert(SUCCEEDED(hr));
	

	hr = pDevice->CreateRenderTargetView
	(
		back_buffer,
		nullptr,
		&pRender_Target_View
	);

	assert(SUCCEEDED(hr));



	// ���� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ���� ����ü�� �ۼ��մϴ�
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ������ ���̹��� ����ü�� ����Ͽ� ���� ���� �ؽ��ĸ� �����մϴ�
	hr = pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);

	assert(SUCCEEDED(hr));
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ���� ����ü�� �ۼ��մϴ�
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ� ������ ���ٽ� �����Դϴ�
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ� �޸��� ���ٽ� �����Դϴ�
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	assert(SUCCEEDED(hr));
	pDevice_Context->OMSetDepthStencilState(m_depthStencilState, 1);
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �並 �����մϴ�
	hr = pDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	assert(SUCCEEDED(hr));

	pDevice_Context->OMSetRenderTargets(1, &pRender_Target_View, m_depthStencilView);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	hr = pDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);
	assert(SUCCEEDED(hr));

	pDevice_Context->RSSetState(m_rasterState);

	m_View_Port.TopLeftX = 0.0f;
	m_View_Port.TopLeftY = 0.0f;
	m_View_Port.Width = static_cast<float>(width);
	m_View_Port.Height = static_cast<float>(height);
	m_View_Port.MinDepth = 0.0f;
	//���̴� 1���� ���� �ȴ�.
	m_View_Port.MaxDepth = 1.0f;

	// ����Ʈ�� �����մϴ�
	pDevice_Context->RSSetViewports(1, &m_View_Port);

	//��������� ���������ֱ�.
	SAFE_RELEASE(back_buffer);
}

void Graphic::Begin()
{
	

	pDevice_Context->ClearRenderTargetView(pRender_Target_View, m_Clear_Color);

	pDevice_Context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphic::End()
{
	auto hr = pSwap_Chain->Present //�ĸ���۸� -> ��������
	(
		1, 
		0  
	);

	assert(SUCCEEDED(hr));
}

void Graphic::TurnZBufferOn()
{
	pDevice_Context->OMSetDepthStencilState(m_depthStencilState, 1);
}


void Graphic::TurnZBufferOff()
{
}