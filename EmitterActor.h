#pragma once

#include "Actor.h"

class EmitterActor : public Actor
{
public:
	EmitterActor(class Scene* scene);
	virtual ~EmitterActor();

	virtual void update(float deltaTime) override;
	virtual void draw() override;

private:


};
