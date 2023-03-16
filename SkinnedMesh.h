#pragma once
#include <assimp/Importer.hpp>  
#include <assimp/scene.h>       
#include <assimp/postprocess.h> 

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
};

struct st_Bone_Data
{
	D3DXMATRIX bones[100];
};

struct MeshEntry {
	MeshEntry()
	{
		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
	}

	UINT NumIndices;
	UINT BaseVertex;
	UINT BaseIndex;
};

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

	void AddBoneData(UINT BoneID, float Weight)
	{
		for (UINT i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
			if (Weights[i] == 0.0)
			{
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}

		// should never get here - more bones than we have space for
		//assert(0);
	}
};

class SkinnedMesh
{
public:

	static SkinnedMesh& GetInstance()
	{
		static SkinnedMesh instance;
		return instance;
	}

	std::vector<D3DXVECTOR3> Positions;
	std::vector<UINT> Indices;

	SkinnedMesh() {};
	~SkinnedMesh();

	bool ReadFile(const std::string& Filename);
	bool LoadMesh(const std::string& Filename);
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(UINT MeshIndex,
		const aiMesh* paiMesh,
		std::vector<D3DXVECTOR3>& Positions,
		std::vector<D3DXVECTOR3>& Normals,
		std::vector<D3DXVECTOR2>& TexCoords,
		std::vector<VertexBoneData>& Bones,
		std::vector<unsigned int>& Indices,
		std::vector<unsigned int>& MeshId );
	void LoadBones(UINT MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	bool SetTexture(std::string path, UINT index);


	void Update(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj);
	void Render();

private:
	const aiScene* m_pScene = nullptr;
	Assimp::Importer m_Importer;
	std::vector<st_vertex_SKINNED> m_vertexSkinned;
	std::vector<UINT> m_idicesSkinned;
	std::vector<ID3D11ShaderResourceView*> m_vecTexture;
	std::map<std::string, UINT> m_BoneMapping;
	UINT m_NumBones;
	std::vector<BoneInfo> m_vecBoneInfo;
	st_Bone_Data m_BoneData;
	ID3D11Buffer * m_BoneBuffer;
	

	std::vector<MeshEntry> m_Entries;

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer * m_pIndicesBuffer = nullptr;

public:
	ID3D11Buffer* GetVBuffer() { return m_pVertexBuffer; };
	ID3D11Buffer* GetIBuffer() { return m_pIndicesBuffer; };
	ID3D11Buffer* GetBoneBuffer() { return m_BoneBuffer; };
	std::vector<ID3D11ShaderResourceView*> GetTextures() { return m_vecTexture; };
	std::vector<MeshEntry>* GetEntry() { return &m_Entries; };
	UINT GetNumMaterials() { return m_pScene->mNumMaterials; };
	UINT GetNumBones() { return m_NumBones; };
	const aiScene* GetScene() { return m_pScene; };
	std::map<std::string, UINT> GetBoneMapping() { return m_BoneMapping; };
	std::vector<BoneInfo> GetBoneInfo() { return m_vecBoneInfo; };
};

