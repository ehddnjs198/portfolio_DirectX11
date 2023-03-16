#pragma once

#include "Shader/cShaderBase.h"

class cShaderTexture : public cShaderBase
{
public:
	cShaderTexture();
	virtual ~cShaderTexture();

public:
	virtual void Initialize(const std::string &shaderFile, const std::string & shaderResource) override;
	//virtual void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	virtual void Render(UINT drawCount) override;


protected:
	ID3D11ShaderResourceView * m_lpShaderResource = nullptr;
	ID3D11SamplerState * m_lpSamplerState = nullptr;
	// cShaderBase을(를) 통해 상속됨
	virtual void InitializeInputLayer(ID3DBlob * m_VS_Blob) override;
	void InitializeShaderResource(const std::string & textureFile);

};

