
#pragma once

#include "SphereActor.h"

class BallActor : public SphereActor
{
public:
	BallActor(class Scene* scene,
		int shaderIndex, float radius,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d);
	~BallActor();

	void update(float deltaTime) override;
	void draw() override;

	Sphere getBoundingSphere() { return Sphere(m_pos, m_radius); }

protected:
	float m_radius;

};

