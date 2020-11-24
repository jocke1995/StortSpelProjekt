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
	m_UploadMatrices.reserve(MAX_ANIMATION_MATRICES);

	DirectX::XMFLOAT4X4 matIdentity;
	DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());

	for (unsigned int i = 0; i < MAX_ANIMATION_MATRICES; i++)
	{
		m_UploadMatrices.push_back(matIdentity);
	}

	// Store the globalInverse transform.
	DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&rootNode->defaultTransform);
	globalInverse = DirectX::XMMatrixInverse(nullptr, globalInverse);
	DirectX::XMStoreFloat4x4(&m_GlobalInverseTransform, globalInverse);

	// Default animation is 'idle'. This will play the idle animation.
	ResetAnimation();
}

AnimatedModel::~AnimatedModel()
{
	delete m_pSkeleton;
	delete m_pCB;
}

void AnimatedModel::InitConstantBuffer(ID3D12Device5* device5, DescriptorHeap* CBV_UAV_SRV_heap)
{
	std::string temp = to_string(m_Path);
	temp = temp.substr(temp.find_last_of("/\\") + 1);
	std::wstring resourceName = L"CB_Matrices_" + to_wstring(temp);

	m_pCB = new ConstantBuffer(device5, sizeof(ANIMATION_MATRICES_STRUCT), resourceName, CBV_UAV_SRV_heap);
}

const ConstantBuffer* AnimatedModel::GetConstantBuffer() const
{
	return m_pCB;
}

const std::vector<DirectX::XMFLOAT4X4>* AnimatedModel::GetUploadMatrices() const
{
	return &m_UploadMatrices;
}

bool AnimatedModel::AddActiveAnimation(std::string animationName, bool loop)
{
	for (auto& animation : m_pActiveAnimations)
	{
		if (animation->name == animationName)
		{
			return false;
		}
	}

	for (auto& animation : m_pPendingAnimations)
	{
		if (animation->name == animationName)
		{
			return false;
		}
	}

	for (auto& animation : m_pEndingAnimations)
	{
		if (animation->name == animationName)
		{
			return false;
		}
	}

	for (auto& animation : m_Animations)
	{
		if (animation->name == animationName)
		{
			animation->loop = loop;
			animation->Update(0);
			if (m_pActiveAnimations.empty())
			{
				m_pActiveAnimations.push_back(animation);
				bindAnimation(m_pSkeleton, animation);
			}
			else
			{
				m_pPendingAnimations.push_back(animation);
				blendAnimations(0);
				bindBlendedAnimation(m_pSkeleton);
			}
			return true;
		}
	}

	return false;
}

bool AnimatedModel::EndActiveAnimation(std::string animationName)
{
	unsigned int index = 0;
	for (auto& animation : m_pActiveAnimations)
	{
		if (animation->name == animationName)
		{
			m_pActiveAnimations.erase(m_pActiveAnimations.begin() + index);
			m_pEndingAnimations.push_back(animation);
			// If there are no active animations left, run default animation.
			if (m_pActiveAnimations.empty())
			{
				ResetAnimation();
			}
			return true;
		}
		index++;
	}

	return false;
}

void AnimatedModel::Update(double dt)
{
	if (!m_AnimationIsPaused)
	{
		unsigned int index = 0;
		for (auto& animation : m_pActiveAnimations)
		{
			// remove all finished animations from the active animations vector
			if (animation->finished)
			{
				//if (m_pActiveAnimations.size() == 1 && m_pPendingAnimations.empty() && m_pEndingAnimations.empty())
				//{
				//	bindAnimation(m_pSkeleton, m_pActiveAnimations[0]);
				//}
				m_pEndingAnimations.push_back(animation);
				m_pActiveAnimations.erase(m_pActiveAnimations.begin() + index);
				animation->finished = false;
			}
			else
			{
				animation->Update(dt);
			}
			index++;
		}

		for (auto& animation : m_pPendingAnimations)
		{
			animation->Update(dt);
		}

		for (auto& animation : m_pEndingAnimations)
		{
			animation->Update(dt);
		}

		if (!m_pPendingAnimations.empty() && !m_pEndingAnimations.empty())
		{
			blendAnimations(dt);
		}

		// If there is no active animation, run the default animation (idle)
		if (m_pActiveAnimations.empty())
		{
			ResetAnimation();
		}

		updateSkeleton(m_pSkeleton, DirectX::XMMatrixIdentity());
	}
}

void AnimatedModel::PlayAnimation()
{
	// bool for indicating if the animation is active or not.
	std::pair<bool, Animation*> idle = { false, nullptr };
	std::pair<bool, Animation*> walk = { false, nullptr };
	std::pair<bool, Animation*> attack = { false, nullptr };

	if (m_pActiveAnimations.size() == 1)
	{
		Animation* animation = m_pActiveAnimations.front();
		bindAnimation(m_pSkeleton, animation);
	}
	else
	{
		// check for active blending
		for (auto& animation : m_pActiveAnimations)
		{
			if (animation->name == "Idle")
			{
				idle.first = true;
				idle.second = animation;
			}
			else if (animation->name == "Walk")
			{
				walk.first = true;
				walk.second = animation;
			}
			else if (animation->name == "Claw_attack_left")
			{
				attack.first = true;
				attack.second = animation;
			}
		}

		// If the animations are to be blended, bind the skeleton in some hardcoded way.
		if (walk.first && attack.first)
		{
			SkeletonNode* hips = findNode(m_pSkeleton, "Hips");
			assert(hips != nullptr);
			bindAnimation(hips, walk.second);

			SkeletonNode* spine = findNode(m_pSkeleton, "Spine");
			assert(spine != nullptr);
			bindAnimation(spine, attack.second);
		}
	}

	m_AnimationIsPaused = false;
}

void AnimatedModel::PauseAnimation()
{
	m_AnimationIsPaused = true;
}

void AnimatedModel::ResetAnimation()
{
	m_pActiveAnimations.clear();
	AddActiveAnimation("Idle", true);
	PlayAnimation();
}

void AnimatedModel::blendAnimations(double dt)
{
	// It would be possible to blend combo-animations (walk+attack) into a third animation (sprint) if we save the state pointers in the skeleton and use them as the interpolation startingpoint.
	if (!m_pPendingAnimations.empty() && !m_pEndingAnimations.empty())
	{
		float factor;
		blendTimeElapsed += dt;
		if (blendTimeElapsed >= blendTransitionTime)
		{
			//if (m_pActiveAnimations.empty())
			//{
			//	ResetAnimation();
			//}
			blendTimeElapsed = 0.0f;
			m_pActiveAnimations.push_back(m_pPendingAnimations[0]);	// The blend phase is finished, so the pending animation will be active now.
			m_pPendingAnimations.pop_back();	// Remove from the pending vector
			m_pEndingAnimations.pop_back();
			bindAnimation(m_pSkeleton, m_pActiveAnimations[0]);
			return;
		}
		else
		{
			factor = blendTimeElapsed / blendTransitionTime;
		}
		assert(factor >= 0.0f && factor <= 1.0f);

		for (auto& key : m_pPendingAnimations[0]->currentState)
		{
			m_BlendAnimationState[key.first].position = InterpolateTranslation(&(m_pEndingAnimations[0]->currentState[key.first].position), &key.second.position, factor);
			m_BlendAnimationState[key.first].rotationQuaternion = InterpolateRotation(&(m_pEndingAnimations[0]->currentState[key.first].rotationQuaternion), &key.second.rotationQuaternion, factor);
			m_BlendAnimationState[key.first].scaling = InterpolateScaling(&(m_pEndingAnimations[0]->currentState[key.first].scaling), &key.second.scaling, factor);
		}
	}
}

void AnimatedModel::bindBlendedAnimation(SkeletonNode* node)
{
	if (m_BlendAnimationState.find(node->name) != m_BlendAnimationState.end())
	{
		node->currentStateTransform = &m_BlendAnimationState[node->name];
	}
	else
	{
		node->currentStateTransform = nullptr;
	}

	// Loop through all nodes in the tree
	for (auto& child : node->children)
	{
		bindBlendedAnimation(child);
	}
}

void AnimatedModel::bindAnimation(SkeletonNode* node, Animation* animation)
{
	if (animation->currentState.find(node->name) != animation->currentState.end())
	{
		node->currentStateTransform = &animation->currentState[node->name];
	}
	else
	{
		node->currentStateTransform = nullptr;
	}

	// Loop through all nodes in the tree
	for (auto& child : node->children)
	{
		bindAnimation(child, animation);
	}
}

void AnimatedModel::updateSkeleton(SkeletonNode* node, DirectX::XMMATRIX parentTransform)
{
	DirectX::XMMATRIX localTransform = DirectX::XMMatrixIdentity();

	if (node->currentStateTransform) // node animated?
	{
		DirectX::XMVECTOR position, rotationQ, scale, rotationOrigin;

		if (node->currentStateTransform->pScale)
		{
			scale = DirectX::XMLoadFloat3(node->currentStateTransform->pScale);
			localTransform *= DirectX::XMMatrixScalingFromVector(scale);
		}

		if (node->currentStateTransform->pRotation)
		{
			rotationQ = DirectX::XMLoadFloat4(node->currentStateTransform->pRotation);
			localTransform *= DirectX::XMMatrixRotationQuaternion(rotationQ);
		}

		if (node->currentStateTransform->pPosition)
		{
			position = DirectX::XMLoadFloat3(node->currentStateTransform->pPosition);
			localTransform *= DirectX::XMMatrixTranslationFromVector(position);
		}
	}
	else // use src transformation otherwise
	{
		localTransform = DirectX::XMLoadFloat4x4(&node->defaultTransform);
	}

	DirectX::XMMATRIX finalTransform = localTransform * parentTransform;

	if (node->boneID != -1)
	{
		DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
		DirectX::XMMATRIX inverseBindPose = DirectX::XMLoadFloat4x4(&node->inverseBindPose);

		DirectX::XMStoreFloat4x4(&m_UploadMatrices[node->boneID], DirectX::XMMatrixTranspose(globalInverse * inverseBindPose * finalTransform));// *transform; //globalInverse * 
	}

	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateSkeleton(node->children[i], finalTransform);
	}
}

SkeletonNode* AnimatedModel::findNode(SkeletonNode* root, std::string nodeName)
{
	SkeletonNode* node;
	if (root->name == nodeName)
	{
		return root;
	}
	else
	{
		for (auto& child : root->children)
		{
			node = findNode(child, nodeName);
			if (node != nullptr)
			{
				return node;
			}
		}

		return nullptr;
	}
}
