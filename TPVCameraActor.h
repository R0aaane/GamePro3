
#pragma once

#include "TrackingCameraActor.h"

class TPVCameraActor : public TrackingCameraActor
{
public:
	TPVCameraActor(class Scene* scene,
		const Camera& cam, Actor3d* target, float angleSpeed = 90.0f,
		float tilt = 30.0f, float back = 3.0f, float height = 2.5f);
	virtual ~TPVCameraActor();

	virtual void update(float deltaTime) override;

	void setOperatable(bool flag) { m_operatable = flag; }
	bool isOperatable() const { return m_operatable; }
	void resetPose();

protected:
	float m_tpvAngle;
	float m_tpvAngleSpeed;
	float m_tilt;
	float m_back;
	float m_height;

	bool  m_operatable;

};




