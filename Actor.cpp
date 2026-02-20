
#include "Actor.h"
#include "Scene.h"

Actor::Actor(Scene* scene)
	: m_scene(scene)
	, m_dead(false)
	, m_visible(true)
	, m_maxSpeed(0.0f)
	, m_lifeTime(1.0f)
{
}

Actor::~Actor()
{
}

void updateActors(std::vector<Actor*>& actors, float deltaTime)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		if ((*it) == nullptr) continue;
		(*it)->update(deltaTime);
	}
}

void drawActors(std::vector<Actor*>& actors)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		if ((*it) == nullptr) continue;
		(*it)->draw();
	}
}

void releaseActors(std::vector<Actor*>& actors)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		if ((*it) != nullptr) delete (*it);
	}
	actors.clear();
}

void removeActors(std::vector<Actor*>& actors, bool erase)
{
	auto it = actors.begin();
	while (it != actors.end())
	{
		if ((*it) != nullptr && (*it)->isDead())
		{
			delete (*it);
			(*it) = nullptr;
			if (erase == true)
			{
				it = actors.erase(it);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

void moveActors(std::vector<Actor*>& temp, std::vector<Actor*>& actors)
{
	for (auto it = temp.begin(); it != temp.end(); ++it)
	{
		actors.push_back(*it);
	}
	temp.clear();
}

void Actor::damage(float dm)
{
}

