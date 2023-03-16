#include "stdafx.h"
#include "MeshAnimator.h"
//#include "SkinnedMesh.h"
#include "Graphic/Graphic.h"

MeshAnimator::~MeshAnimator()
{
	for (int i = 0; i < m_vecAddedMesh.size(); i++)
	{
		SAFE_DELETE(m_vecAddedMesh[i]);
		m_vecAddedMesh.clear();
	}
}

bool MeshAnimator::Init()
{
	m_NumBones = SkinnedMesh::GetInstance().GetNumBones();
	m_pScene = SkinnedMesh::GetInstance().GetScene();
	m_vecBoneInfo = SkinnedMesh::GetInstance().GetBoneInfo();
	m_BoneMapping = SkinnedMesh::GetInstance().GetBoneMapping();
	m_BoneBuffer = SkinnedMesh::GetInstance().GetBoneBuffer();

	m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
	m_GlobalInverseTransform.Inverse();

	for (int i = 0; i < m_pScene->mNumAnimations; i++)
		m_AnimationClips.push_back(m_pScene->mAnimations[i]);

	

	return true;
}

void MeshAnimator::Upadate()
{
	static float f;
	f += 0.01f;

	
	BoneTransform(f);

	for (unsigned int i = 0; i < m_vecBoneInfo.size(); i++) {

		D3DXMATRIX mat;

		mat._11 = m_vecBoneInfo[i].FinalTransformation.a1;
		mat._12 = m_vecBoneInfo[i].FinalTransformation.a2;
		mat._13 = m_vecBoneInfo[i].FinalTransformation.a3;
		mat._14 = m_vecBoneInfo[i].FinalTransformation.a4;

		mat._21 = m_vecBoneInfo[i].FinalTransformation.b1;
		mat._22 = m_vecBoneInfo[i].FinalTransformation.b2;
		mat._23 = m_vecBoneInfo[i].FinalTransformation.b3;
		mat._24 = m_vecBoneInfo[i].FinalTransformation.b4;

		mat._31 = m_vecBoneInfo[i].FinalTransformation.c1;
		mat._32 = m_vecBoneInfo[i].FinalTransformation.c2;
		mat._33 = m_vecBoneInfo[i].FinalTransformation.c3;
		mat._34 = m_vecBoneInfo[i].FinalTransformation.c4;

		mat._41 = m_vecBoneInfo[i].FinalTransformation.d1;
		mat._42 = m_vecBoneInfo[i].FinalTransformation.d2;
		mat._43 = m_vecBoneInfo[i].FinalTransformation.d3;
		mat._44 = m_vecBoneInfo[i].FinalTransformation.d4;


		m_BoneData.bones[i] = mat;
	}

	D3D11_MAPPED_SUBRESOURCE	    mapped_subresource;
	Graphic::GetInstance().GetDeviceContext()->Map
	(
		m_BoneBuffer,				//어떤 리소스를 갱신 할것인가.
		0,							//서브 리소스 몰라도됨
		D3D11_MAP_WRITE_DISCARD,	//맵 타입 -> 쓰고 파괴
		0,							//맵 플러그인 0 안씀
		&mapped_subresource			//리소스.
	);

	memcpy(
		mapped_subresource.pData,	//목적 파일, 복사본
		&m_BoneData,				//원본파일
		sizeof(st_Bone_Data));	//크기.

	Graphic::GetInstance().GetDeviceContext()->Unmap
	(
		m_BoneBuffer, 0
	);

	Graphic::GetInstance().GetDeviceContext()->VSSetConstantBuffers
	(
		1, 1, &m_BoneBuffer
	);
}

void MeshAnimator::BoneTransform(float TimeInSeconds)
{
	aiMatrix4x4 Identity;

	float TicksPerSecond = (float)(m_pScene->mAnimations[m_NumCurrentAnimation]->mTicksPerSecond !=
		0 ? m_pScene->mAnimations[m_NumCurrentAnimation]->mTicksPerSecond : 30.0f);

	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[m_NumCurrentAnimation]->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);
}

void MeshAnimator::addAnimationClip(std::string path)
{
	SkinnedMesh* mesh = new SkinnedMesh;
	mesh->ReadFile(path);
	m_vecAddedMesh.push_back(mesh);
	
	for (int i = 0; i < mesh->GetScene()->mNumAnimations; i++)
	{
		m_AnimationClips.push_back(mesh->GetScene()->mAnimations[i]);
	}
}

void MeshAnimator::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
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

void MeshAnimator::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
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

void MeshAnimator::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
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

UINT MeshAnimator::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
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

UINT MeshAnimator::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
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

UINT MeshAnimator::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	return 0;
}

const aiNodeAnim * MeshAnimator::FindNodeAnim(const aiAnimation * pAnimation, const std::string NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (pNodeAnim->mNodeName.data == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void MeshAnimator::ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, const aiMatrix4x4 & ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[m_NumCurrentAnimation];

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling, ScalingM);

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
		m_vecBoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_vecBoneInfo[BoneIndex].BoneOffset;
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}
