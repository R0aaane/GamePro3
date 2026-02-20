
#pragma once

#include "SphereActor.h"
#include "CubeActor.h"

class ContactableActor : public CubeActor
{
public:
	ContactableActor(class Scene* scene,
		int shaderIndex, std::vector<const wchar_t*> texturePath,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d);
	~ContactableActor();

	void update(float deltaTime) override;
	void draw() override;

	OBB3d getBoundingBox() { return OBB3d(m_pos, getForward(),
		getRight(), getUp(), 0.5f * m_scale); }
	AABB3d getAABB() { return m_aabb; }

protected:
	AABB3d m_aabb;

};





