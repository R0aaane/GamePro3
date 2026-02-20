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

	if (key.isPressed('S'))
	{
		int w = m_scene->getGame()->getWidth();
		int h = m_scene->getGame()->getHeight();
		float x = (float)m_scene->getGame()->getRand(30, w- 30);
		float y = (float)m_scene->getGame()->getRand(30, h - 30);

		std::vector<UINT> indices{ 1, 0, 1, 2 };
		std::vector<std::vector<UINT>> anims{ indices };
		ItemActor* act = new ItemActor(m_scene,
			L"src\\pipo-hikarimono007.png", 16.0f, anims, 0, 0.2f, 3, 4,
			Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(x, y));
		if (!act->isEnabled()) throw std::exception();

		MyGameScene* scene = (MyGameScene*)m_scene;
		scene->addItem(act);
	}
}	

void EmitterActor::draw()
{

} 