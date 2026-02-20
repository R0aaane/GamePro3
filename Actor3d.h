
#pragma once

#include "Actor.h"

class Actor3d : public Actor
{
public:
	Actor3d(class Scene* scene, 
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d);
	virtual ~Actor3d();

	virtual void updateNode(float deltaTime, const XMMATRIX& parentMat);

	virtual void setPos(XMFLOAT3 pos);
	XMFLOAT3 getPos() const { return m_pos; }
	virtual void setVel(XMFLOAT3 vel) { m_vel = vel; }
	XMFLOAT3 getVel() const { return m_vel; }

	virtual void setAngle(XMFLOAT3 angle);
	XMFLOAT3 getAngle() const { return m_angle; }
	virtual void setAngleVel(XMFLOAT3 avel) { m_angleVel = avel; }
	XMFLOAT3 getAngleVel() const { return m_angleVel; }

	void setPivot(XMFLOAT3 pivot);
	XMFLOAT3 getPivot() const { return m_pivot; }

	virtual void setScale(XMFLOAT3 scale);
	XMFLOAT3 getScale() const { return m_scale; }

	XMFLOAT3 getForward();
	XMFLOAT3 getRight();
	XMFLOAT3 getUp();

	XMMATRIX getWorldMatrix() const { return m_worldTransform; }
	XMMATRIX getRigidTransformationMatrix();

	void setMass(float mass) { m_mass = mass; }
	float getMass() { return m_mass; }
	void setGravitationalAcceleration(float g) { m_gaccel = g; }
	float getGravitationalAcceleration() { return m_gaccel; }
	void setEnableGravity(bool gravity) { m_enableGravity = gravity; }
	bool isEnableGravity() { return m_enableGravity; }
	void setDragCoefficient(float coef) { m_dragCoefficient = coef; }
	float getDragCoefficient() { return m_dragCoefficient; }
	void setMaxSpeed(float s) { m_maxSpeed = s; }
	float getMaxSpeed() { return m_maxSpeed; }

protected:
	XMFLOAT3 m_pos;
	XMFLOAT3 m_vel;
	XMFLOAT3 m_accel;
	XMFLOAT3 m_angle;
	XMFLOAT3 m_angleVel;
	XMFLOAT3 m_pivot;
	XMFLOAT3 m_scale;
	XMMATRIX m_worldTransform;
	bool m_recalculateWorldTransform;

	float m_mass;
	float m_gaccel;
	bool m_enableGravity;
	float m_dragCoefficient;
	float m_maxSpeed;

	void calcWorldTransform();

	void translate(float deltaTime);
	void rotate(float deltaTime);
	void simulate(float deltaTime);
	void simulatePhysics(float deltaTime, XMFLOAT3 addForce = ZeroVec3d);

};

// ÉmÅ[Éhä«óù
struct ActorNode
{
	ActorNode* parent;
	std::vector<ActorNode*> children;
	Actor3d* actor;

	ActorNode() : parent(nullptr), actor(nullptr) {}
	ActorNode(Actor3d* act) : parent(nullptr), actor(act) {}
	void addNode(ActorNode* node);
};

void updateActorNode(ActorNode* node, float deltaTime);
void drawActorNode(ActorNode* node);
void releaseActorNode(ActorNode* node);


