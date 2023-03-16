#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

//#include "Texture/cTexture.h"

#define NUM_BONES_PER_VEREX 4

struct st_vertex_SKINNED
{
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_normal;
	D3DXVECTOR2 m_texcoord;
	UINT m_boneId[NUM_BONES_PER_VEREX];
	float m_weights[NUM_BONES_PER_VEREX];
};

struct BoneInfo
{
	aiMatrix4x4 BoneOffset;
	aiMatrix4x4 FinalTransformation;

	BoneInfo()
	{
		//BoneOffset.SetZero();
		//FinalTransformation.SetZero();
	}
};

class cSkinnedMesh
{
public:
	cSkinnedMesh();
	~cSkinnedMesh();

	bool LoadMesh(const std::string& Filename);
	void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	void Render();
	UINT NumBones() const
	{
		return m_NumBones;
	}
	void BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms);

private:
	

	const aiScene* m_pScene;
	std::vector<st_vertex_SKINNED> m_vertexSkinned;
	std::vector<UINT> m_idicesSkinned;

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	//인덱스 버퍼를 만들기 위함.
	//UINT *m_idices = nullptr;
	//UINT m_idicesSize = 0;
	ID3D11Buffer * m_Idices_Buffer = nullptr;

	class cShaderSkinned* m_pSkinnedRenderer = nullptr;
	//class cMeshRenderer* m_pMeshRenderer = nullptr;
	//class c

	struct VertexBoneData
	{
		UINT IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(UINT BoneID, float Weight);
	};

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(UINT MeshIndex,
		const aiMesh* paiMesh,
		std::vector<D3DXVECTOR3>& Positions,
		std::vector<D3DXVECTOR3>& Normals,
		std::vector<D3DXVECTOR2>& TexCoords,
		std::vector<VertexBoneData>& Bones,
		std::vector<unsigned int>& Indices);
	void LoadBones(UINT MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};

	UINT m_VAO;
	UINT m_Buffers[NUM_VBs];

	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	//std::vector<cTexture*> m_Textures;

	std::map<std::string, UINT> m_BoneMapping; // maps a bone name to its index
	UINT m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;
	aiMatrix4x4 m_GlobalInverseTransform;

	//const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};
