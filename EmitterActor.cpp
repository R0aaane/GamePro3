#include "EmitterActor.h"
#include "Game.h"
#include "Scene.h"
#include "MyGameScene.h"
#include "ItemActor.h"

EmitterActor::EmitterActor(Scene* scene)
	: Actor(scene)
{
}

EmitterActor::~EmitterActor()
{
}

void EmitterActor::update(float deltaTime)
{
	const Keyboard& key = m_scene->getGame()->getKeyboard();

	if (key.isDown('S'))
	{
		int h = m_scene->getGame()->getHeight();
		float y = (float)m_scene->getGame()->getRand(30, h - 30);

		std::vector<UINT> indices{ 0, 1, 2 };
		std::vector<UINT> indicesExplode{ 3, 4, 5 };
		std::vector<std::vector<UINT>> anims{ indices, indicesExplode };
		ItemActor* act = new ItemActor(m_scene,
			L"src\\balloon.png", 16.0f, anims, 0, 0.2f, 6, 12,
			Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(640.0f, y), XMFLOAT2(-100.0f, 0.0f));
		if (!act->isEnabled()) throw std::exception();

		MyGameScene* scene = (MyGameScene*)m_scene;
		scene->addItem(act);
	}
}	

void EmitterActor::draw()
{

} 