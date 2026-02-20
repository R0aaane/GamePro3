
#pragma once

#include "TrackingCameraActor.h"

class FPVCameraActor : public TrackingCameraActor
{
public:
	FPVCameraActor(class Scene* scene,
		const Camera& cam, Actor3d* target, const XMFLOAT3& pos = ZeroVec3d,
		float baseAngle = 0.0f, float maxTilt = 60.0f);
	virtual ~FPVCameraActor();

	virtual void update(float deltaTime) override;

	void setOperatable(bool flag) { m_operatable = flag; }
	bool isOperatable() const { return m_operatable; }

protected:
	float m_baseAngle;
	float m_tilt;
	float m_maxTilt;
	bool  m_operatable;

};











