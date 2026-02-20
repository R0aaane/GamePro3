#include "PlayerActor2D.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"
#include "MyGameScene.h"
#include "ItemActor.h"

PlayerActor2D::PlayerActor2D(Scene* scene, const std::wstring & filePath,
	float radius,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, UINT wNum, UINT hNum,
	int shaderIndex,
	const XMFLOAT2& pos, const XMFLOAT2& vel,
	const XMFLOAT2& scale, float angle, float angleVel,
	XMFLOAT2* spriteSize, bool centerFlag, bool ddsFlag)
	: BlockAnimActor(scene, filePath, indices, lane, interval, wNum, hNum,
		shaderIndex, pos, vel, scale, angle, angleVel, spriteSize, centerFlag, ddsFlag)
	, m_radius(radius)
{

}

PlayerActor2D:: ~PlayerActor2D()
{

}

void PlayerActor2D::update(float deltaTime)
{
	const Keyboard& key = m_scene->getGame()->getKeyboard();

	int  x = 0, y = 0;
	if (key.isDown(VK_LEFT)) --x;
	if (key.isDown(VK_RIGHT)) ++x;
	if (key.isDown(VK_UP)) --y;
	if (key.isDown(VK_DOWN)) ++y;
	XMFLOAT2 v = 200.0f * normalize(XMFLOAT2((float)x, (float)y));
	setVel(v);
	simulate(deltaTime);

	float w = (float)m_scene->getGame()->getWidth();
	float h = (float)m_scene->getGame()->getHeight();
	if (m_pos.x - m_radius < 0.0f)m_pos.x = m_radius;
	if (m_pos.y - m_radius < 0.0f)m_pos.y = m_radius;
	if (m_pos.x + m_radius >= w)m_pos.x = w - m_radius;
	if (m_pos.y + m_radius >= h)m_pos.y = h - m_radius;

	BlockAnimActor::update(deltaTime);

	MyGameScene* scene = (MyGameScene*)m_scene;
	std::vector<Actor*>& items = scene->getItems();
	Circle c = getCircle();
	for (int i = 0; i <items.size(); ++i)
	{
		ItemActor* item = (ItemActor*)items[i];
		Circle ci = item->getCircle();
		if (detectCircleCollision(c, ci))
		{
			if (!item->isExploding())
			{
				item->explode();
			}
		}
	}
}

void PlayerActor2D::draw()
{
	BlockAnimActor::draw();
}