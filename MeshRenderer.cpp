#include "stdafx.h"
#include "MeshRenderer.h"
#include "SkinnedMesh.h"
#include "Graphic/Graphic.h"

MeshRenderer::~MeshRenderer()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_VS_Blob);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_PS_Blob);
	SAFE_RELEASE(m_Transform_Buffer);
	SAFE_RELEASE(m_pRasterizerState);
}

bool MeshRenderer::Init()
{
	bool ret = true;

	m_pVerticesBuffer = SkinnedMesh::GetInstance().GetVBuffer();
	m_pIndicesBuffer = SkinnedMesh::GetInstance().GetIBuffer();

	if (!m_pVerticesBuffer || !m_pIndicesBuffer)
		assert(0);

	{//Vertex Shader
		auto hr = D3DX11CompileFromFileA 
		(
			"TestShader.hlsl", 
			nullptr,
			nullptr,
			"VS",
			"vs_5_0",
			0,
			0,
			nullptr, 
			&m_VS_Blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = Graphic::GetInstance().GetDevice()->CreateVertexShader
		(
			m_VS_Blob->GetBufferPointer(),
			m_VS_Blob->GetBufferSize(),
			nullptr,
			&m_pVertexShader
		);

		assert(SUCCEEDED(hr));
	}

	{//InputLayout
		D3D11_INPUT_ELEMENT_DESC input_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,0, D3D11_INPUT_PER_VERTEX_DATA },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,12, D3D11_INPUT_PER_VERTEX_DATA },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0,24, D3D11_INPUT_PER_VERTEX_DATA },
			{ "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT,
			0,32, D3D11_INPUT_PER_VERTEX_DATA },
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,48, D3D11_INPUT_PER_VERTEX_DATA }
		};

		auto hr = Graphic::GetInstance().GetDevice()->CreateInputLayout
		(
			input_desc,
			5,
			m_VS_Blob->GetBufferPointer(),
			m_VS_Blob->GetBufferSize(),
			&m_pInputLayout	
		);

		assert(SUCCEEDED(hr));
	}

	

	return ret;
}

void MeshRenderer::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{

	if (!BOX->isPicking)
	{
		{//Pixel Shader
			auto hr = D3DX11CompileFromFileA
			(
				"TestShader.hlsl",
				nullptr,
				nullptr,
				"PS",
				"ps_5_0",
				0,
				0,
				nullptr,
				&m_PS_Blob,
				nullptr,
				nullptr
			);

			assert(SUCCEEDED(hr));

			hr = Graphic::GetInstance().GetDevice()->CreatePixelShader
			(
				m_PS_Blob->GetBufferPointer(),
				m_PS_Blob->GetBufferSize(),
				nullptr,
				&m_pPixelShader
			);

			assert(SUCCEEDED(hr));
		}

		{//ConstantBuffer
			D3D11_BUFFER_DESC matrixBufferDesc;
			ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
			matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			matrixBufferDesc.ByteWidth = sizeof(st_Transform_Data);
			matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			matrixBufferDesc.MiscFlags = 0;
			matrixBufferDesc.StructureByteStride = 0;

			auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_Transform_Buffer);
			assert(SUCCEEDED(hr));
		}
		if (!BOX->isWireFrame)
		{
			{//Create Rasterizer State
				D3D11_RASTERIZER_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

				desc.FillMode = D3D11_FILL_SOLID;
				desc.CullMode = D3D11_CULL_BACK;
				desc.FrontCounterClockwise = false;

				auto hr = Graphic::GetInstance().GetDevice()->CreateRasterizerState
				(
					&desc, &m_pRasterizerState
				);
				assert(SUCCEEDED(hr));
			}
		}
		else
		{
			{//Create Rasterizer State
				D3D11_RASTERIZER_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

				desc.FillMode = D3D11_FILL_WIREFRAME;
				desc.CullMode = D3D11_CULL_BACK;
				desc.FrontCounterClockwise = false;

				auto hr = Graphic::GetInstance().GetDevice()->CreateRasterizerState
				(
					&desc, &m_pRasterizerState
				);
				assert(SUCCEEDED(hr));
			}
		}
		
	}

	if (BOX->isPicking)
	{
		{//Pixel Shader
			auto hr = D3DX11CompileFromFileA
			(
				"TestShader.hlsl",
				nullptr,
				nullptr,
				"PS1",
				"ps_5_0",
				0,
				0,
				nullptr,
				&m_PS_Blob,
				nullptr,
				nullptr
			);

			assert(SUCCEEDED(hr));

			hr = Graphic::GetInstance().GetDevice()->CreatePixelShader
			(
				m_PS_Blob->GetBufferPointer(),
				m_PS_Blob->GetBufferSize(),
				nullptr,
				&m_pPixelShader
			);

			assert(SUCCEEDED(hr));
		}

		{//ConstantBuffer
			D3D11_BUFFER_DESC matrixBufferDesc;
			ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
			matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			matrixBufferDesc.ByteWidth = sizeof(st_Transform_Data);
			matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			matrixBufferDesc.MiscFlags = 0;
			matrixBufferDesc.StructureByteStride = 0;

			auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_Transform_Buffer);
			assert(SUCCEEDED(hr));
		}

		{//Create Rasterizer State
			D3D11_RASTERIZER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_BACK;
			desc.FrontCounterClockwise = false;

			auto hr = Graphic::GetInstance().GetDevice()->CreateRasterizerState
			(
				&desc, &m_pRasterizerState
			);
			assert(SUCCEEDED(hr));
		}
	}

	D3DXMatrixTranspose(&m_stTransform_Data.m_Mat_Word, &world);
	D3DXMatrixTranspose(&m_stTransform_Data.m_Mat_View, &view);
	D3DXMatrixTranspose(&m_stTransform_Data.m_Mat_Projection, &proj);

	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_Transform_Buffer,			
		0,							
		D3D11_MAP_WRITE_DISCARD,	
		0,							
		&mapped_subresource			
	);

	memcpy
	(
		mapped_subresource.pData,	
		&m_stTransform_Data,		
		sizeof(st_Transform_Data)
	);	

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_Transform_Buffer, 0
	);
}

void MeshRenderer::Render()
{
	Graphic::GetInstance().GetDeviceContext()->IASetInputLayout(m_pInputLayout);

	Graphic::GetInstance().GetDeviceContext()->VSSetShader
	(
		m_pVertexShader, nullptr, 0
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		0, 1, &m_Transform_Buffer
	);

	

	Graphic::GetInstance().GetDeviceContext()->RSSetState
	(m_pRasterizerState);

	Graphic::GetInstance().GetDeviceContext()->PSSetShader
	(
		m_pPixelShader, nullptr, 0
	);

	UINT stride = sizeof(st_vertex_SKINNED);
	UINT offset = 0;

	Graphic::GetInstance().GetDeviceContext()->IASetVertexBuffers
	(
		0,
		1,
		&m_pVerticesBuffer,
		&stride,
		&offset
	);

	Graphic::GetInstance().GetDeviceContext()->IASetIndexBuffer
	(
		m_pIndicesBuffer,
		DXGI_FORMAT_R32_UINT,
		0
	);

	Graphic::GetInstance().GetDeviceContext()->IASetPrimitiveTopology
	(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	
	for (int i = 0; i < SkinnedMesh::GetInstance().GetEntry()->size(); i++)
	{
		
		Graphic::GetInstance().GetDeviceContext()->PSSetShaderResources
		(
			0, 1, &SkinnedMesh::GetInstance().GetTextures()[i]
		);
		
		int baseVertexLocation = SkinnedMesh::GetInstance().GetEntry()->at(i).BaseVertex;
		UINT startIndexLocation = SkinnedMesh::GetInstance().GetEntry()->at(i).BaseIndex;
		UINT indexCount = SkinnedMesh::GetInstance().GetEntry()->at(i).NumIndices;
		Graphic::GetInstance().GetDeviceContext()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
}
