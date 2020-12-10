#include "stdafx.h"
#include "structs.h"
#include "AnimatedModel.h"
#include "GPUMemory/ConstantBuffer.h"

#include "DescriptorHeap.h"

AnimatedModel::AnimatedModel(
	const std::wstring* path,
	SkeletonNode* rootNode,
	std::vector<Mesh*>* meshes,
	std::vector<Animation*>* animations,
	std::vector<Material*>* materials,
	unsigned int numBones)
	: Model(path, meshes, materials)
{
	m_Id = s_AnimatedModelIdCounter++;

	m_pSkeleton = rootNode;
	m_Animations = (*animations);
}

AnimatedModel::~AnimatedModel()
{
	delete m_pSkeleton;
}

SkeletonNode* AnimatedModel::CloneSkeleton()
{
	return m_pSkeleton->Clone();
}

std::vector<Animation*> AnimatedModel::GetAnimations()
{
	return m_Animations;
}
