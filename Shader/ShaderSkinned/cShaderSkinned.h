#pragma once

#include "Shader/cShaderBase.h"
#include "SkinnedMesh/cSkinnedMesh.h"

struct ST_BONETransform_DATA
{
	D3DXMATRIX bones[100];
};

class cShaderSkinned : public cShaderBase
{
public:
	cShaderSkinned();
	virtual ~cShaderSkinned();

public:
	virtual void Initialize(const std::string &shaderFile, const std::string & shaderResource) override;
	virtual void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	virtual void Render(UINT drawCount) override;

	void SkinnedUpdate(std::vector<BoneInfo> *,D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);

protected:
	ST_BONETransform_DATA m_BoneData;
	ID3D11Buffer*		  m_BoneTransform_Buffer;

	ID3D11ShaderResourceView * m_lpShaderResource = nullptr;
	ID3D11SamplerState * m_lpSamplerState = nullptr;
	// cShaderBase을(를) 통해 상속됨
	virtual void InitializeInputLayer(ID3DBlob * m_VS_Blob) override;
	void InitializeShaderResource(const std::string & textureFile);

};