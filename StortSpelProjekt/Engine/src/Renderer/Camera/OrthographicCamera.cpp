#include "stdafx.h"
#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(
	DirectX::XMVECTOR position, DirectX::XMVECTOR direction,
	float left, float right,
	float bot, float top,
	float nearZ, float farZ)
	:BaseCamera(position, direction)
{
	m_Left = left;
	m_Right = right;
	m_Bot = bot;
	m_Top = top;
	m_NearZ = nearZ;
	m_FarZ = farZ;

	updateProjectionMatrix();

	updateSpecific(0);
}
OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::updateProjectionMatrix()
{
	m_ProjMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		m_Left,
		m_Right,
		m_Bot,
		m_Top,
		m_NearZ,
		m_FarZ);
}

void OrthographicCamera::updateSpecific(double dt)
{
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

const float OrthographicCamera::GetLeft() const
{
	return m_Left;
}

const float OrthographicCamera::GetRight() const
{
	return m_Right;
}

const float OrthographicCamera::GetBot() const
{
	return m_Bot;
}

const float OrthographicCamera::GetTop() const
{
	return m_Top;
}

const float OrthographicCamera::GetNearZ() const
{
	return m_NearZ;
}

const float OrthographicCamera::GetFarZ() const
{
	return m_FarZ;
}

void OrthographicCamera::SetLeft(float left)
{
	m_Left = left;
	updateProjectionMatrix();
}

void OrthographicCamera::SetRight(float right)
{
	m_Right = right;
	updateProjectionMatrix();
}

void OrthographicCamera::SetBot(float bot)
{
	m_Bot = bot;
	updateProjectionMatrix();
}

void OrthographicCamera::SetTop(float top)
{
	m_Top = top;
	updateProjectionMatrix();
}

void OrthographicCamera::SetNearZ(float nearPlaneDistance)
{
	m_NearZ = nearPlaneDistance;
	updateProjectionMatrix();
}

void OrthographicCamera::SetFarZ(float farPlaneDistance)
{
	m_FarZ = farPlaneDistance;
	updateProjectionMatrix();
}
