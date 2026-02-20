
#include "ContactableActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

ContactableActor::ContactableActor(Scene* scene,
	int shaderIndex, std::vector<const wchar_t*> texturePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale)
	: CubeActor(scene, shaderIndex, texturePath, true,
		pos, vel, angle, avel, scale)
{
	setOriginalMaterial(MeshMaterial(Ones3d, 0.08f * Ones3d, 8.0f, 0.2f * Ones3d));

	std::vector<XMFLOAT3> points(2);
	points[0] = 0.5f * m_scale;
	points[1] = -0.5f * m_scale;
	m_aabb.initialize(points);
}

ContactableActor::~ContactableActor()
{
}

void ContactableActor::update(float deltaTime)
{
	simulate(deltaTime);
	calcWorldTransform();
	m_aabb.update(m_worldTransform);
}

void ContactableActor::draw()
{
	if (m_visible == false) return;

	CubeActor::draw();
}






