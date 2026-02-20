#include "PlayerActor2D.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"
#include "MyGameScene.h"

PlayerActor2D::PlayerActor2D(Scene* scene, const std::wstring & filePath,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, UINT wNum, UINT hNum,
	int shaderIndex,
	const XMFLOAT2& pos, const XMFLOAT2& vel,
	const XMFLOAT2& scale, float angle, float angleVel,
	XMFLOAT2* spriteSize, bool centerFlag, bool ddsFlag)
	: BlockAnimActor(scene, filePath, indices, lane, interval, wNum, hNum,
		shaderIndex, pos, vel, scale, angle, angleVel, spriteSize, centerFlag, ddsFlag)
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

	BlockAnimActor::update(deltaTime);
}

void PlayerActor2D::draw()
{
	BlockAnimActor::draw();
}