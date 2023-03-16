#pragma once

#include "Shader/cShaderBase.h"

class cShaderColor : public cShaderBase
{
public:
	cShaderColor();
	virtual ~cShaderColor();

public:
	virtual void Initialize(const std::string &shaderFile);
	virtual void Render(UINT drawCount) override;
protected:
	// cShaderBase을(를) 통해 상속됨
	virtual void InitializeInputLayer(ID3DBlob * m_VS_Blob) override;
};

