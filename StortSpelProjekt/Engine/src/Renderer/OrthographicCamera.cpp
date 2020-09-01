#include "stdafx.h"
#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, float left, float right, float bot, float top, float nearZ, float farZ)
	:BaseCamera(position, lookAt)
{
	this->left = left;
	this->right = right;
	this->bot = bot;
	this->top = top;
	this->nearZ = nearZ;
	this->farZ = farZ;
	this->projMatrix = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bot, top, nearZ, farZ);
	this->UpdateSpecific(0);
}
OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::UpdateSpecific(double dt)
{
	this->projMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		this->left,
		this->right,
		this->bot,
		this->top,
		this->nearZ,
		this->farZ);

	this->viewProjMatrix = this->m_ViewMatrix * this->projMatrix;
	this->viewProjTranposedMatrix = DirectX::XMMatrixTranspose(this->viewProjMatrix);
}

const DirectX::XMMATRIX* OrthographicCamera::GetViewProjection() const
{
	return &this->viewProjMatrix;
}

const DirectX::XMMATRIX* OrthographicCamera::GetViewProjectionTranposed() const
{
	return &this->viewProjTranposedMatrix;
}
