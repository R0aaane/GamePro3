
#pragma once

#include "Actor3d.h"

class StarSystemActor : public Actor3d
{
public:
	StarSystemActor(class Scene* scene, 
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& angle = ZeroVec3d);
	~StarSystemActor();

	void update(float deltaTime) override;
	void draw() override;

private:
	ActorNode* m_rootNode;

};






