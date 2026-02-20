
#pragma once

#include "Actor3d.h"

class RoboActor : public Actor3d
{
public:
	RoboActor(class Scene* scene,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& angle = ZeroVec3d);
	~RoboActor();

	void update(float deltaTime) override;
	void draw() override;

private:
	ActorNode* m_rootNode;

};


