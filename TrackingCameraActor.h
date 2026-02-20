
#pragma once

#include "CameraActor.h"

class TrackingCameraActor : public CameraActor
{
public:
	TrackingCameraActor(class Scene* scene,
		const Camera& cam, Actor3d* target,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& angle = ZeroVec3d);
	virtual ~TrackingCameraActor();

	virtual void update(float deltaTime) override;

	virtual void setPos(XMFLOAT3 pos) override;
	virtual void setAngle(XMFLOAT3 angle) override;

protected:
	Actor3d* m_target;
	XMMATRIX m_offset;

};








