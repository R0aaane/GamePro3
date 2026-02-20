
#pragma once

#include "SphereActor.h"
#include "CubeActor.h"
#include "StringActor.h"
#include "SpriteActor.h"
#include "FileMeshActor.h"
#include "FileAnimMeshActor.h"

class PlayerActor : public FileAnimMeshActor
{
public:
	PlayerActor(class Scene* scene,
		int shaderIndex, float radius, const wchar_t* filePath,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d);
	~PlayerActor();

	void update(float deltaTime) override;
	void draw() override;

	Sphere getBoundingSphere() { return Sphere(m_pos + m_radius * UnitVecZ3d, m_radius); }
	OBB3d getBoundingBox() { return OBB3d(m_pos, getForward(),
		getRight(), getUp(), 0.5f * m_scale); }

	enum class MoveState
	{
		Idle,
		Walk,
		Jump
	};
	MoveState getMoveState() { return m_moveState; }

private:
	float m_radius;

	MoveState m_moveState;
	float m_moveForce;
	float m_rotateSpeed;
	float m_jumpSpeed;
	float m_jumpTime;
	float m_jumpAssistTime;
	float m_fallForce;
	XMFLOAT3 m_jumpVector;
	float m_permissionHeight;

	std::unique_ptr<SpriteActor> m_reticle;
	bool  m_reticleMode;
	float m_reticleSpeed;
	float m_reticleRadius;

	SpotLight m_spotLight;
	int       m_spotLightIndex;
	XMFLOAT3  m_lightPos;
	XMFLOAT3  m_lightVec;

	std::unique_ptr<StringActor> m_str;
	FontData m_font;

};





