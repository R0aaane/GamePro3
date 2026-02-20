
#include "Actor3d.h"
#include "Game.h"

Actor3d::Actor3d(Scene* scene, const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale)
	: Actor(scene)
	, m_pos(pos)
	, m_vel(vel)
	, m_accel(ZeroVec3d)
	, m_angle(angle)
	, m_angleVel(avel)
	, m_pivot(ZeroVec3d)
	, m_scale(scale)
	, m_worldTransform(XMMatrixIdentity())
	, m_recalculateWorldTransform(true)
	, m_mass(1.0f)
	, m_gaccel(9.8f)
	, m_enableGravity(true)
	, m_dragCoefficient(0.0f)
	, m_maxSpeed(50.0f)
{
	calcWorldTransform();
}

Actor3d::~Actor3d()
{
}

void Actor3d::setPos(XMFLOAT3 pos)
{
	m_pos = pos;
	m_recalculateWorldTransform = true;
}

void Actor3d::setAngle(XMFLOAT3 angle)
{
	m_angle = angle;
	m_angle.x = (m_angle.x >= 360.0f) ? std::fmod(m_angle.x, 360.0f)
		: ((m_angle.x < 0.0f) ? 360.0f - std::fmod(std::fabs(m_angle.x), 360.0f) : m_angle.x);
	m_angle.y = (m_angle.y >= 360.0f) ? std::fmod(m_angle.y, 360.0f)
		: ((m_angle.y < 0.0f) ? 360.0f - std::fmod(std::fabs(m_angle.y), 360.0f) : m_angle.y);
	m_angle.z = (m_angle.z >= 360.0f) ? std::fmod(m_angle.z, 360.0f)
		: ((m_angle.z < 0.0f) ? 360.0f - std::fmod(std::fabs(m_angle.z), 360.0f) : m_angle.z);

	m_recalculateWorldTransform = true;
}

void Actor3d::setPivot(XMFLOAT3 pivot)
{
	m_pivot = pivot;
	m_recalculateWorldTransform = true;
}

void Actor3d::setScale(XMFLOAT3 scale)
{
	m_scale = scale;
	m_recalculateWorldTransform = true;
}

XMFLOAT3 Actor3d::getForward()
{
	calcWorldTransform();

	float rx = m_worldTransform.r[0].m128_f32[0];
	float ry = m_worldTransform.r[0].m128_f32[1];
	float rz = m_worldTransform.r[0].m128_f32[2];

	return normalize(XMFLOAT3(rx, ry, rz));
}

XMFLOAT3 Actor3d::getRight()
{
	calcWorldTransform();

	float rx = m_worldTransform.r[1].m128_f32[0];
	float ry = m_worldTransform.r[1].m128_f32[1];
	float rz = m_worldTransform.r[1].m128_f32[2];

	return normalize(XMFLOAT3(rx, ry, rz));
}

XMFLOAT3 Actor3d::getUp()
{
	calcWorldTransform();

	float rx = m_worldTransform.r[2].m128_f32[0];
	float ry = m_worldTransform.r[2].m128_f32[1];
	float rz = m_worldTransform.r[2].m128_f32[2];

	return normalize(XMFLOAT3(rx, ry, rz));
}

void Actor3d::calcWorldTransform()
{
	if (m_recalculateWorldTransform == false) return;

	m_worldTransform = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	m_worldTransform *= XMMatrixTranslation(-m_pivot.x, -m_pivot.y, -m_pivot.z);
	m_worldTransform *= XMMatrixRotationX(XMConvertToRadians(m_angle.x));
	m_worldTransform *= XMMatrixRotationY(XMConvertToRadians(m_angle.y));
	m_worldTransform *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
	m_worldTransform *= XMMatrixTranslation(m_pos.x + m_pivot.x,
		m_pos.y + m_pivot.y, m_pos.z + m_pivot.z);

	m_recalculateWorldTransform = false;
}

void Actor3d::translate(float deltaTime)
{
	setPos(m_pos + m_vel * deltaTime);
}

void Actor3d::rotate(float deltaTime)
{
	setAngle(m_angle + m_angleVel * deltaTime);
}

void Actor3d::simulate(float deltaTime)
{
	rotate(deltaTime);
	calcWorldTransform();
	translate(deltaTime);
}

XMMATRIX Actor3d::getRigidTransformationMatrix()
{
	XMMATRIX mat = m_worldTransform;

	float sx = 1.0f / m_scale.x;
	float sy = 1.0f / m_scale.y;
	float sz = 1.0f / m_scale.z;

	mat.r[0].m128_f32[0] *= sx;
	mat.r[0].m128_f32[1] *= sx;
	mat.r[0].m128_f32[2] *= sx;

	mat.r[1].m128_f32[0] *= sy;
	mat.r[1].m128_f32[1] *= sy;
	mat.r[1].m128_f32[2] *= sy;

	mat.r[2].m128_f32[0] *= sz;
	mat.r[2].m128_f32[1] *= sz;
	mat.r[2].m128_f32[2] *= sz;

	return mat;
}

void Actor3d::simulatePhysics(float deltaTime, XMFLOAT3 addForce)
{
	XMFLOAT3 f = addForce;
	if (m_enableGravity) f += -m_mass * m_gaccel * UnitVecZ3d;
	f += -m_dragCoefficient * m_vel;

	XMFLOAT3 a = (1.0f / m_mass) * f;
	XMFLOAT3 v = m_vel + deltaTime * 0.5f * (m_accel + a);
	XMFLOAT3 p = m_pos + deltaTime * m_vel + 0.5f * deltaTime * deltaTime * m_accel;

	m_accel = a;
	float s = length(v);
	if (s > m_maxSpeed)
	{
		v = m_maxSpeed * normalize(v);
	}
	m_vel = v;
	setPos(p);
	calcWorldTransform();
}

void Actor3d::updateNode(float deltaTime, const XMMATRIX& parentMat)
{
}

void ActorNode::addNode(ActorNode* node)
{
	node->parent = this;
	children.push_back(node);
}

void updateActorNode(ActorNode* node, float deltaTime)
{
	if (node == nullptr) return;

	if (node->parent != nullptr) node->actor->updateNode(deltaTime,
		node->parent->actor->getRigidTransformationMatrix());
	for (size_t i = 0; i < node->children.size(); ++i)
	{
		updateActorNode(node->children[i], deltaTime);
	}
}

void drawActorNode(ActorNode* node)
{
	if (node == nullptr) return;

	if (node->parent != nullptr) node->actor->draw();
	for (size_t i = 0; i < node->children.size(); ++i)
	{
		drawActorNode(node->children[i]);
	}
}

void releaseActorNode(ActorNode* node)
{
	if (node == nullptr) return;

	for (size_t i = 0; i < node->children.size(); ++i)
	{
		releaseActorNode(node->children[i]);
	}

	if (node->parent != nullptr && node->actor != nullptr)
	{
		delete node->actor;
	}
	delete node;
}


