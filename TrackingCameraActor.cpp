
#include "TrackingCameraActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

TrackingCameraActor::TrackingCameraActor(Scene* scene,
	const Camera& cam, Actor3d* target,
	const XMFLOAT3& pos, const XMFLOAT3& angle)
	: CameraActor(scene, cam, pos, ZeroVec3d, angle)
	, m_target(target)
{
	m_offset = XMMatrixRotationX(XMConvertToRadians(m_angle.x));
	m_offset *= XMMatrixRotationY(XMConvertToRadians(m_angle.y));
	m_offset *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
	m_offset *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	if (m_target != nullptr)
	{
		m_worldTransform = m_offset * m_target->getRigidTransformationMatrix();
	}
	else
	{
		m_worldTransform = m_offset;
	}
	m_recalculateWorldTransform = false;
}

TrackingCameraActor::~TrackingCameraActor()
{
}

void TrackingCameraActor::update(float deltaTime)
{
	if (m_target != nullptr)
	{
		m_worldTransform = m_offset * m_target->getRigidTransformationMatrix();
	}
}

void TrackingCameraActor::setPos(XMFLOAT3 pos)
{
	m_pos = pos;

	m_offset = XMMatrixRotationX(XMConvertToRadians(m_angle.x));
	m_offset *= XMMatrixRotationY(XMConvertToRadians(m_angle.y));
	m_offset *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
	m_offset *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	if (m_target != nullptr)
	{
		m_worldTransform = m_offset * m_target->getRigidTransformationMatrix();
	}
	else
	{
		m_worldTransform = m_offset;
	}
}

void TrackingCameraActor::setAngle(XMFLOAT3 angle)
{
	m_angle = angle;

	m_offset = XMMatrixRotationX(XMConvertToRadians(m_angle.x));
	m_offset *= XMMatrixRotationY(XMConvertToRadians(m_angle.y));
	m_offset *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
	m_offset *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	if (m_target != nullptr)
	{
		m_worldTransform = m_offset * m_target->getRigidTransformationMatrix();
	}
	else
	{
		m_worldTransform = m_offset;
	}
}



