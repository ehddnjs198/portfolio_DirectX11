#include "stdafx.h"

#include "Graphic/Graphic.h"
#include "SkinnedMesh/cSkinnedMesh.h"
#include "Shader/ShaderSkinned/cShaderSkinned.h"



#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4


void cSkinnedMesh::VertexBoneData::AddBoneData(UINT BoneID, float Weight)
{
	for (UINT i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

}

cSkinnedMesh::cSkinnedMesh()
{
	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_pScene = NULL;
}


cSkinnedMesh::~cSkinnedMesh()
{
	Clear();
}


void cSkinnedMesh::Clear()
{
	SAFE_DELETE(m_pSkinnedRenderer);
	SAFE_RELEASE(m_Idices_Buffer);
	//SAFE_DELETE(m_idices);
	SAFE_RELEASE(m_pVertexBuffer);
}

bool cSkinnedMesh::LoadMesh(const std::string& Filename)
{
	// Release the previously loaded mesh (if it exists)
	//Clear();

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);//오른손 좌표곈지 어캐알지
	if (m_pScene) {
		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		Ret = InitFromScene(m_pScene, Filename);
	}
	else {
		//printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorstd::string());
	}

	// Make sure the VAO is not changed from the outside
	//glBindVertexArray(0);

	return Ret;
}

void cSkinnedMesh::Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	std::vector<aiMatrix4x4> Transforms;

	//float RunningTime = GetRunningTime();

	static float f;
	f += 0.001f;

	if (f >= 1.f)
		f = 0;


	BoneTransform(f, Transforms);//트렌스폼 칭구
	//D3DXMatrixScaling(&world, 0.01f, 0.01f, 0.01f);

	m_pSkinnedRenderer->SkinnedUpdate(&m_BoneInfo, world, view, proj);//쉐이더에서 긁어온친구
	//m_pMeshRenderer->Update(world, view, proj, &m_BoneInfo);
}


bool cSkinnedMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	//m_Textures.resize(pScene->mNumMaterials);

	//1137로 크기가 다 같음.
	std::vector<D3DXVECTOR3> Positions;
	std::vector<D3DXVECTOR3> Normals;
	std::vector<D3DXVECTOR2> TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<UINT> Indices;

	UINT NumVertices = 0;
	UINT NumIndices = 0;

	// Count the number of vertices and indices
	for (UINT i = 0; i < m_Entries.size(); i++) {

		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		//숫자 세는거 같음
		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (UINT i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
	}

	if (!InitMaterials(pScene, Filename)) {
		return false;
	}
	/////////////버퍼 초기화 구간?
	// Generate and populate the buffers with vertex attributes and the indices

	st_vertex_SKINNED skin;
	m_vertexSkinned.reserve(NumVertices);
	m_idicesSkinned.reserve(NumIndices);
	//m_idices = &Indices[0];

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
		desc.ByteWidth = sizeof(st_vertex_SKINNED) * m_vertexSkinned.size();//버택스의 버퍼 크기를 얼만큼 할꺼냐.

		D3D11_SUBRESOURCE_DATA sub_data;
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
			&desc, &sub_data, &m_Idices_Buffer
		);
		assert(SUCCEEDED(hr));
	}

	//m_idicesSize = NumIndices;

	//m_pMeshRenderer = new cMeshRenderer();
	m_pSkinnedRenderer = new cShaderSkinned();
	m_pSkinnedRenderer->Initialize("Shader/ShaderSkinned.hlsl","Resources/fbx/textures/Wolf_Body.jpg");
	return true;
}


void cSkinnedMesh::InitMesh(UINT MeshIndex,
	const aiMesh* paiMesh,
	std::vector<D3DXVECTOR3>& Positions,
	std::vector<D3DXVECTOR3>& Normals,
	std::vector<D3DXVECTOR2>& TexCoords,
	std::vector<VertexBoneData>& Bones,
	std::vector<UINT>& Indices)
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
	}

	LoadBones(MeshIndex, paiMesh, Bones);

	// Populate the index buffer
	for (UINT i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

}


void cSkinnedMesh::LoadBones(UINT MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
	for (UINT i = 0; i < pMesh->mNumBones; i++) {
		UINT BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (UINT j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			UINT VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}

}


bool cSkinnedMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	// Extract the directory part from the file name
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
	
	// Initialize the materials
	for (UINT i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
	
		//m_Textures[i] = NULL;
	
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);
	
				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}
	
				std::string FullPath = Dir + "/" + p;
	
				//std::cout << FullPath << std::endl;

				//m_Textures[i] = new cTexture(GL_TEXTURE_2D, FullPath.c_str());
				//m_Textures[i] = new cTexture();
				//m_Textures[i]->Initialize(FullPath);

				

				//if (!m_Textures[i]->Load()) {
				//	printf("Error loading texture '%s'\n", FullPath.c_str());
				//	delete m_Textures[i];
				//	m_Textures[i] = NULL;
				//	Ret = false;
				//}
				//else {
				//	printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
				//}
			}
		}
	}
	
	return Ret;
}


void cSkinnedMesh::Render()
{

	UINT stride = sizeof(st_vertex_SKINNED);
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
	//Graphic::GetInstance().GetDeviceContext()->IASetInputLayout(m_pInputLayer);

	Graphic::GetInstance().GetDeviceContext()->IASetPrimitiveTopology
	(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	m_pSkinnedRenderer->Render(m_idicesSkinned.size());
	//m_pMeshRenderer->Render(m_idicesSkinned.size(), &m_Textures[0]->GetTexture());

	//for (cTexture* p : m_Textures)
	//{
	//	if (p != NULL)
	//	{
	//
	//	}
	//}

	//glBindVertexArray(m_VAO);
	//
	//for (UINT i = 0; i < m_Entries.size(); i++) {
	//	const UINT MaterialIndex = m_Entries[i].MaterialIndex;
	//
	//	assert(MaterialIndex < m_Textures.size());
	//
	//	if (m_Textures[MaterialIndex]) {
	//		m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
	//	}
	//
	//	glDrawElementsBaseVertex(GL_TRIANGLES,
	//		m_Entries[i].NumIndices,
	//		GL_UNSIGNED_INT,
	//		(void*)(sizeof(UINT) * m_Entries[i].BaseIndex),
	//		m_Entries[i].BaseVertex);
	//}
	//
	//// Make sure the VAO is not changed from the outside    
	//glBindVertexArray(0);

}


UINT cSkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	return 0;
}


UINT cSkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


UINT cSkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


void cSkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	UINT PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	UINT NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void cSkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void cSkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	UINT ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	UINT NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;

	Out = Start + Factor * Delta;
}


void cSkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	//애니메이션 바꾸 바꾸는거
	const aiAnimation* pAnimation = m_pScene->mAnimations[1];

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling, ScalingM);
		//ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		aiMatrix4x4 TranslationM;

		aiMatrix4x4::Translation(Translation, TranslationM);
		//TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		UINT BoneIndex = m_BoneMapping[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


void cSkinnedMesh::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms)
{
	aiMatrix4x4 Identity;

	float TicksPerSecond = (float)(m_pScene->mAnimations[4]->mTicksPerSecond != 0 ? m_pScene->mAnimations[4]->mTicksPerSecond : 30.0f);//쉬버 있으면 그거 돌리고 없으면 30초 때리기

	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[4]->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	for (UINT i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}


const aiNodeAnim* cSkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}