
#pragma once

#include "Actor3d.h"
#include "Camera.h"

class CameraActor : public Actor3d
{
public:
	CameraActor(class Scene* scene, const Camera& cam,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d);
	virtual ~CameraActor();

	virtual void update(float deltaTime) override;
	void draw() override;

	void setCameraParam(const Camera& cam) { m_camera = cam; }
	void setCameraInParam(const CamInParam& cam) { m_camera.setInParam(cam); }
	void setCameraExtParam(const CamExtParam& cam) { m_camera.setExtParam(cam); }
	Camera getCameraParam() const { return m_camera; }

	void uploadParam();
	void uploadInParam();
	void uploadExtParam();

protected:
	Camera m_camera;

};





