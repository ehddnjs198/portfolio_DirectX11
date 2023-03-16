#pragma once

#include "Shader/cShaderBase.h"

class cShaderFire : public cShaderBase
{
private:
	struct NoiseBufferType
	{
		float frameTime;
		D3DXVECTOR3 scrollSpeeds;
		D3DXVECTOR3 scales;
		float padding;
	};

	struct DistortionBufferType
	{
		D3DXVECTOR2 distortion1;
		D3DXVECTOR2 distortion2;
		D3DXVECTOR2 distortion3;
		float distortionScale;
		float distortionBias;
	};
public:
	cShaderFire();
	virtual ~cShaderFire();

private:

	NoiseBufferType m_noiseData;
	ID3D11Buffer* m_noiseBuffer = nullptr;
	
	ID3D11ShaderResourceView* m_ResourceView[3];
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11SamplerState* m_sampleState2 = nullptr;

	DistortionBufferType m_distortionData;
	ID3D11Buffer* m_distortionBuffer = nullptr;

	float frameTime = 0.0f;
	D3DXVECTOR3 scrollSpeeds = D3DXVECTOR3(1.3f, 2.1f, 2.3f);
	D3DXVECTOR3 scales = D3DXVECTOR3(1.0f, 2.0f, 3.0f);

public:

	virtual void Initialize(const std::string &shaderFile) override;
	virtual void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj) override;
	virtual void Render(UINT drawCount) override;
	//void Shutdown();
	//bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
	//	ID3D11ShaderResourceView*, float, XMFLOAT3, XMFLOAT3, XMFLOAT2, XMFLOAT2, XMFLOAT2, float, float);

private:

	// cShaderBase을(를) 통해 상속됨
	virtual void InitializeInputLayer(ID3DBlob * m_VS_Blob) override;


};