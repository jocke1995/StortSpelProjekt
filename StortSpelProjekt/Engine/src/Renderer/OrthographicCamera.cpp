#include "stdafx.h"
#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR direction, float left, float right, float bot, float top, float nearZ, float farZ)
	:BaseCamera(position, direction)
{
	m_Left = left;
	m_Right = right;
	m_Bot = bot;
	m_Top = top;
	m_NearZ = nearZ;
	m_FarZ = farZ;
	m_ProjMatrix = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bot, top, nearZ, farZ);
	updateSpecific(0);
}
OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::updateSpecific(double dt)
{
	m_ProjMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		m_Left,
		m_Right,
		m_Bot,
		m_Top,
		m_NearZ,
		m_FarZ);

	m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;
	m_ViewProjTranposedMatrix = DirectX::XMMatrixTranspose(m_ViewProjMatrix);
}

const DirectX::XMMATRIX* OrthographicCamera::GetViewProjection() const
{
	return &m_ViewProjMatrix;
}

const DirectX::XMMATRIX* OrthographicCamera::GetViewProjectionTranposed() const
{
	return &m_ViewProjTranposedMatrix;
}
