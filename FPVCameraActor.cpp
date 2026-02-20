
#include "FPVCameraActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

FPVCameraActor::FPVCameraActor(Scene* scene,
	const Camera& cam, Actor3d* target, const XMFLOAT3& pos,
	float baseAngle, float maxTilt)
	: TrackingCameraActor(scene, cam, target, pos,
		XMFLOAT3(0.0f, baseAngle, 0.0f))
	, m_baseAngle(baseAngle)
	, m_tilt(0.0f)
	, m_maxTilt(maxTilt)
	, m_operatable(true)
{
}

FPVCameraActor::~FPVCameraActor()
{
}

void FPVCameraActor::update(float deltaTime)
{
	if (m_operatable)
	{
		const Gamepad& pad = m_scene->getGame()->getGamepad(0);
		float tilt = m_maxTilt * ((float)pad.getRightTrigger() / 255.0f);
		m_tilt = m_baseAngle - tilt;
		m_angle = XMFLOAT3(0.0f, m_tilt, 0.0f);

		m_offset = XMMatrixRotationY(XMConvertToRadians(m_angle.y));
		m_offset *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	}

	if (m_target != nullptr)
	{
		m_worldTransform = m_offset * m_target->getRigidTransformationMatrix();
	}
}


