
#include "TPVCameraActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

TPVCameraActor::TPVCameraActor(Scene* scene,
	const Camera& cam, Actor3d* target, float angleSpeed,
	float tilt, float back, float height)
	: TrackingCameraActor(scene, cam, target, XMFLOAT3(-back, 0.0f, height),
		XMFLOAT3(0.0f, tilt, 0.0f))
	, m_tpvAngle(0.0f)
	, m_tpvAngleSpeed(angleSpeed)
	, m_tilt(tilt)
	, m_back(back)
	, m_height(height)
	, m_operatable(true)
{
}

TPVCameraActor::~TPVCameraActor()
{
}

void TPVCameraActor::update(float deltaTime)
{
	if (m_operatable)
	{
		const Gamepad& pad = m_scene->getGame()->getGamepad(0);
		if (pad.isPressed(XINPUT_GAMEPAD_START))
		{
			resetPose();
		}
		else
		{
			int r = 0;
			if (pad.isDown(XINPUT_GAMEPAD_LEFT_SHOULDER))  ++r;
			if (pad.isDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) --r;
			if (r != 0)
			{
				m_tpvAngle += r * m_tpvAngleSpeed * deltaTime;
				m_tpvAngle = (m_tpvAngle >= 360.0f) ? m_tpvAngle - 360.0f : m_tpvAngle;
				m_tpvAngle = (m_tpvAngle < 0.0f) ? m_tpvAngle + 360.0f : m_tpvAngle;

				float angle = XMConvertToRadians(m_tpvAngle);
				m_pos = XMFLOAT3(-m_back * XMScalarCos(angle),
					-m_back * XMScalarSin(angle), m_height);
				m_angle = XMFLOAT3(0.0f, m_tilt, m_tpvAngle);

				m_offset = XMMatrixRotationY(XMConvertToRadians(m_angle.y));
				m_offset *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
				m_offset *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
			}
		}
	}

	if (m_target != nullptr)
	{
		m_worldTransform = m_offset * m_target->getRigidTransformationMatrix();
	}
}

void TPVCameraActor::resetPose()
{
	m_tpvAngle = 0.0f;
	m_pos = XMFLOAT3(-m_back, 0.0f, m_height);
	m_angle = XMFLOAT3(0.0f, m_tilt, 0.0f);

	m_offset = XMMatrixRotationY(XMConvertToRadians(m_angle.y));
	m_offset *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
	m_offset *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}

