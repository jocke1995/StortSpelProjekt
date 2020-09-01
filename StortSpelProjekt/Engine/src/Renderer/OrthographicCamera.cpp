#include "stdafx.h"
#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, float left, float right, float bot, float top, float nearZ, float farZ)
	:BaseCamera(position, lookAt)
{
	this->m_Left = left;
	this->m_Right = right;
	this->m_Bot = bot;
	this->m_Top = top;
	this->m_NearZ = nearZ;
	this->m_FarZ = farZ;
	this->m_ProjMatrix = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bot, top, nearZ, farZ);
	this->updateSpecific(0);
}
OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::updateSpecific(double dt)
{
	this->m_ProjMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		this->m_Left,
		this->m_Right,
		this->m_Bot,
		this->m_Top,
		this->m_NearZ,
		this->m_FarZ);

	this->m_ViewProjMatrix = this->m_ViewMatrix * this->m_ProjMatrix;
	this->m_ViewProjTranposedMatrix = DirectX::XMMatrixTranspose(this->m_ViewProjMatrix);
}

const DirectX::XMMATRIX* OrthographicCamera::GetViewProjection() const
{
	return &this->m_ViewProjMatrix;
}

const DirectX::XMMATRIX* OrthographicCamera::GetViewProjectionTranposed() const
{
	return &this->m_ViewProjTranposedMatrix;
}
