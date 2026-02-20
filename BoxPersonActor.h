
#pragma once

#include "Actor3d.h"

class BoxPersonActor : public Actor3d
{
public:
	BoxPersonActor(class Scene* scene,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& angle = ZeroVec3d);
	~BoxPersonActor();

	void update(float deltaTime) override;
	void draw() override;

private:
	ActorNode* m_rootNode;

	float m_interval;
	float m_time;
	float m_moveTime;
	float m_footAngle;
	float m_armAngle;

	enum class MoveState
	{
		Idle,
		Walk
	};
	MoveState m_state;

	void walk(float deltaTime);
	void idle();

};











