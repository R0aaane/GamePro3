
#include "BallActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"
#include "TestScene.h"
#include "TerrainActor.h"

BallActor::BallActor(Scene* scene,
	int shaderIndex, float radius,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel)
	: SphereActor(scene, shaderIndex, Renderer::WhiteTexture, true,
		pos, vel, angle, avel, 2.0f * radius * Ones3d)
	, m_radius(radius)
{
	setOriginalMaterial(MeshMaterial(XMFLOAT3(0.95f, 0.5f, 0.0f),
		XMFLOAT3(0.6f, 0.3f, 0.0f), 8.0f, XMFLOAT3(0.2f, 0.1f, 0.0f)));
}


BallActor::~BallActor()
{
}

void BallActor::update(float deltaTime)
{
	//simulatePhysics(deltaTime, 10.0f * UnitVecY3d);
	simulatePhysics(deltaTime);

	TestScene* scene = (TestScene*)m_scene;
	TerrainActor* terrain = scene->getTerrain();
	//Sphere s = getBoundingSphere();
	//Plane p = terrain->getBoundingPlane();
	XMFLOAT3 corr = -1.0f * UnitVecZ3d;
	Ray ray(m_pos, corr);
	float len = 0.0f;
	GridHeightMap map = terrain->getBoundingGridHeightMap();
	if (detectRayToGridMeshCollision(ray, map, &len))
	{
		setPos(m_pos + (len - m_radius + 10.0f * FLT_EPSILON) * corr);
		calcWorldTransform();
		m_vel = ZeroVec3d;
		m_accel = ZeroVec3d;
	}
}

void BallActor::draw()
{
	if (m_visible == false) return;

	SphereActor::draw();
}


