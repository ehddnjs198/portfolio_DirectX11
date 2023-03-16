#pragma once
class Graphic final
	//final = �� �̻� ��� �� �� ���� �����. ����ȭ �Ұ�.
{
private:
	Graphic();
public:

	static Graphic& GetInstance()
	{
		static Graphic instance;
		return instance;
	}

	~Graphic();

	//�ʱ�ȭ
	void Initialize();
	//����۸� �����.
	void CreateBackBuffer(const UINT &width, const UINT &height);

	auto GetDevice() -> ID3D11Device* { return pDevice; }
	auto GetDeviceContext() ->ID3D11DeviceContext* { return pDevice_Context; }

	void Begin();
	void End();
	//�׸��⸦ �����ϰ� ������ �ϴ� �Լ���.
	void TurnZBufferOn();
	void TurnZBufferOff();


private:
	ID3D11Device * pDevice = nullptr;
	//
	ID3D11DeviceContext * pDevice_Context = nullptr;
	IDXGISwapChain * pSwap_Chain = nullptr;
	ID3D11RenderTargetView * pRender_Target_View = nullptr;

	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;

	D3D11_VIEWPORT m_View_Port = { 0 };
	D3DXCOLOR m_Clear_Color = 0xff555566; //ȸ������.
};