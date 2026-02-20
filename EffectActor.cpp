
#include "EffectActor.h"
#include "Game.h"
#include "Scene.h"

EffectActor::EffectActor(Scene* scene, const std::wstring& filePath,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, UINT wNum, UINT hNum,
	int shaderIndex, const XMFLOAT2& pos, const XMFLOAT2& vel,
	const XMFLOAT2& scale, float angle, float angleVel,
	XMFLOAT2* spriteSize, bool centerFlag, bool ddsFlag)
	: BlockAnimActor(scene, filePath, indices, lane, interval, wNum, hNum,
		shaderIndex, pos, vel, scale, angle, angleVel,
		spriteSize, centerFlag, ddsFlag)
	, m_delayTime(0.0f)
{
}

EffectActor::~EffectActor()
{
}

void EffectActor::update(float deltaTime)
{
	if (m_delayTime > 0.0f)
	{
		m_delayTime -= deltaTime;
		return;
	}

	simulate(deltaTime);
	BlockAnimActor::updateAnim(deltaTime);
	if (getAnimState() == AnimState::Stop)
	{
		setDead();
	}
}

void EffectActor::draw()
{
	if (m_delayTime > 0.0f) return;

	SpriteActor::draw();
}

// 複数のエフェクトを生成
void createMultipleEffects(Scene* scene, const std::wstring& filePath,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, UINT wNum, UINT hNum, int shaderIndex,
	UINT eNum, float eInterval, float eVel,	float eWidth, float eHeight,
	const XMFLOAT2& basePos, const XMFLOAT2& scale,
	XMFLOAT2* spriteSize, bool centerFlag, bool ddsFlag)
{
	float angle = (float)(360.0f * scene->getGame()->getRand());
	EffectActor* effect = new EffectActor(scene, filePath, indices, lane, interval,
		wNum, hNum, shaderIndex, basePos, ZeroVec2d, scale, angle, 0.0f,
		spriteSize, centerFlag);
	effect->setAnimState(1, true);
	scene->addActor(effect);

	for (UINT i = 0; i < eNum; ++i)
	{
		float dx = -eWidth + 2.0f * eWidth * (float)scene->getGame()->getRand();
		float dy = -eHeight + 2.0f * eHeight * (float)scene->getGame()->getRand();
		XMFLOAT2 d = XMFLOAT2(dx, dy);
		XMFLOAT2 v = normalize(d) * eVel;
		angle = (float)(360.0f * scene->getGame()->getRand());
		float delay = (float)(i + 1) * eInterval;

		effect = new EffectActor(scene, filePath, indices, lane, interval,
			wNum, hNum, shaderIndex, basePos + d, v, scale, angle, 0.0f,
			spriteSize, centerFlag);
		effect->setAnimState(1, true);
		effect->setDelayTime(delay);
		scene->addActor(effect);
	}
}







