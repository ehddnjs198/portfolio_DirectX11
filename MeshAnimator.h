#pragma once

#include <assimp/postprocess.h> 
#include <assimp/scene.h>
#include "SkinnedMesh.h"



class MeshAnimator
{
private:
	

public:
	MeshAnimator() {};
	~MeshAnimator();

	bool Init();
	void Upadate();

	void addAnimationClip(std::string path);
	void SetAniNum(UINT num) { m_NumCurrentAnimation = num; };

private:
	void BoneTransform(float TimeInSeconds);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);

private:
	aiMatrix4x4 m_GlobalInverseTransform;

	std::vector<BoneInfo> m_vecBoneInfo;
	std::map<std::string, UINT> m_BoneMapping;

	const aiScene* m_pScene;
	UINT m_NumBones;
	UINT m_NumCurrentAnimation = 0;
	std::vector<aiAnimation*> m_AnimationClips;
	std::vector<SkinnedMesh*> m_vecAddedMesh;
	
	st_Bone_Data m_BoneData;
	ID3D11Buffer * m_BoneBuffer;

};

