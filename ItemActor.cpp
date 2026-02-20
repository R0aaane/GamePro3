#include "ItemActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"
#include "MyGameScene.h"

ItemActor::ItemActor(Scene* scene, const std::wstring& filePath,
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

ItemActor:: ~ItemActor()
{

}

void ItemActor::update(float deltaTime)
{
	BlockAnimActor::update(deltaTime);
	float explodeTime = 0.5f;

	if (m_exploding)
	{
		explodeTimer += deltaTime;

		if (explodeTimer >= explodeTime)
		{
			setDead();
		}
	}
}

void ItemActor::draw()
{
	BlockAnimActor::draw();
}

void ItemActor::explode()
{
	if (m_exploding) return;
	m_exploding = true;
	explodeTimer = 0.0f;

	// 爆発アニメーションのレーンに切り替える
	changeLane(1);
}