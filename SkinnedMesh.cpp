#include "stdafx.h"
#include "SkinnedMesh.h"
#include "Graphic/Graphic.h"


SkinnedMesh::~SkinnedMesh()
{
	SAFE_RELEASE(m_pIndicesBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	 
	for (int i = 0; i < m_vecTexture.size(); i++)
		SAFE_RELEASE(m_vecTexture[i]);
	m_vecTexture.clear();
}

bool SkinnedMesh::ReadFile(const std::string & Filename)
{
	bool ret = false;
	if (m_pScene = m_Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS))
		ret = true;

	return ret;
}

bool SkinnedMesh::LoadMesh(const std::string & Filename)
{
	bool Ret = false;

	if (!ReadFile(Filename))
		return false;
	
	Ret = InitFromScene(m_pScene, Filename);

	return Ret;
}

bool SkinnedMesh::InitFromScene(const aiScene * pScene, const std::string & Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_vecTexture.resize(pScene->mNumMeshes);
	m_NumBones = 0;

	

	std::vector<D3DXVECTOR3> Normals;
	std::vector<D3DXVECTOR2> TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<UINT> MeshIndex;

	UINT NumVertices = 0;
	UINT NumIndices = 0;

	for (UINT i = 0; i < m_Entries.size(); i++) {

		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);
	

	for (UINT i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices,MeshIndex);
	}

	if (!InitMaterials(pScene, Filename)) {
	}

	st_vertex_SKINNED skin;
	m_vertexSkinned.reserve(NumVertices);
	m_idicesSkinned.reserve(NumIndices);

	m_idicesSkinned = Indices;

	for (int i = 0; i < NumVertices; ++i)
	{
		skin.m_position = Positions[i];
		skin.m_texcoord = TexCoords[i];
		skin.m_normal = Normals[i];

		for (int j = 0; j < 4; ++j)
		{
			skin.m_boneId[j] = Bones[i].IDs[j];
			skin.m_weights[j] = Bones[i].Weights[j];
		}

		m_vertexSkinned.push_back(skin);
	}


	{//Vertex Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//어느 스테이지에서 쓸것이냐.
		desc.ByteWidth = sizeof(st_vertex_SKINNED) * m_vertexSkinned.size(); 
																			 

		D3D11_SUBRESOURCE_DATA sub_data;
		//CPU데이터를 GPU에 연결 할 수 있다.
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));

		sub_data.pSysMem = &m_vertexSkinned[0];

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc,	//버퍼에 대한 설명서.
			&sub_data, //초기화 데이터.
			&m_pVertexBuffer //버텍스 버퍼.
		);

		assert(SUCCEEDED(hr));
	}

	{//Index Buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(UINT) * Indices.size();
		//
		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = &m_idicesSkinned[0];
		//
		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer
		(
			&desc, &sub_data, &m_pIndicesBuffer
		);
		assert(SUCCEEDED(hr));
	}

	{
		D3D11_BUFFER_DESC matrixBufferDesc;
		ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(st_Bone_Data);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		auto hr = Graphic::GetInstance().GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_BoneBuffer);

		for (int i = 0; i < 100; i++)
			D3DXMatrixIdentity(&m_BoneData.bones[i]);

		D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
		Graphic::GetInstance().GetDeviceContext()->Map
		(
			m_BoneBuffer,				
			0,							
			D3D11_MAP_WRITE_DISCARD,	
			0,							
			&mapped_subresource			
		);

		memcpy(
			mapped_subresource.pData,	
			&m_BoneData,				
			sizeof(st_Bone_Data));	

		Graphic::GetInstance().GetDeviceContext()->Unmap
		(
			m_BoneBuffer, 0
		);

		Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
		(
			1, 1, &m_BoneBuffer
		);
	}

	return true;
}

void SkinnedMesh::InitMesh(UINT MeshIndex, const aiMesh * paiMesh, std::vector<D3DXVECTOR3>& Positions, std::vector<D3DXVECTOR3>& Normals, std::vector<D3DXVECTOR2>& TexCoords, std::vector<VertexBoneData>& Bones, std::vector<unsigned int>& Indices, std::vector<unsigned int>& TexNum)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (UINT i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(D3DXVECTOR3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(D3DXVECTOR3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(D3DXVECTOR2(pTexCoord->x, 1 - pTexCoord->y));
		TexNum.push_back(MeshIndex);
	}

	LoadBones(MeshIndex, paiMesh, Bones);

	// Populate the index buffer
	for (UINT i = 0; i < paiMesh->mNumFaces; i++) 
	{
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

}

void SkinnedMesh::LoadBones(UINT MeshIndex, const aiMesh * paiMesh, std::vector<VertexBoneData>& Bones)
{

	for (UINT i = 0; i < paiMesh->mNumBones; i++) {
		UINT BoneIndex = 0;
		std::string BoneName(paiMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_vecBoneInfo.push_back(bi);
			m_vecBoneInfo[BoneIndex].BoneOffset = paiMesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (UINT j = 0; j < paiMesh->mBones[i]->mNumWeights; j++) {
			UINT VertexID = m_Entries[MeshIndex].BaseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

bool SkinnedMesh::InitMaterials(const aiScene * pScene, const std::string & Filename)
{
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	for (UINT i = 0; i < pScene->mNumMaterials; i++) 
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		


		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
		{
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string p(Path.data);

				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				std::string FullPath = Dir + "/" + p;
				ID3D11ShaderResourceView* srv;

				auto hr = D3DX11CreateShaderResourceViewFromFileA
				(
					Graphic::GetInstance().GetDevice(),
					FullPath.c_str(),
					nullptr,
					nullptr,
					&srv,
					nullptr
				);

				if (SUCCEEDED(hr))
					m_vecTexture[i] = srv;
				else
					Ret = false;
			}
		}
	}

	return Ret;
}

bool SkinnedMesh::SetTexture(std::string path, UINT index)
{
	ID3D11ShaderResourceView* srv;
	

	auto hr = D3DX11CreateShaderResourceViewFromFileA
	(
		Graphic::GetInstance().GetDevice(),
		path.c_str(),
		nullptr,
		nullptr,
		&srv,
		nullptr
	);

	if (!SUCCEEDED(hr))
		return false;

	if (m_vecTexture[index])
		SAFE_RELEASE(m_vecTexture[index]);

	m_vecTexture[index] = srv;

	return true;
}

void SkinnedMesh::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
}

void SkinnedMesh::Render()
{
}
