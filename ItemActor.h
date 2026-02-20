#pragma once

#include "BlockAnimActor.h"

class ItemActor : public BlockAnimActor
{
public:
	ItemActor(class Scene* scene, const std::wstring& filePath,
		float radius,
		const std::vector<std::vector<UINT>>& indices,
		UINT lane, float interval, UINT wNum = 3, UINT hNum = 4,
		int shaderIndex = Renderer::Shader2DAlphaLoopPoint,
		const XMFLOAT2& pos = ZeroVec2d, const XMFLOAT2& vel = ZeroVec2d,
		const XMFLOAT2& scale = Ones2d, float angle = 0.0f, float angleVel = 0.0f,
		XMFLOAT2* spriteSize = nullptr, bool centerFlag = true, bool ddsFlag = false);
	virtual ~ItemActor();

	virtual void update(float deltaTime) override;
	void draw() override;

	Circle getCircle() { return Circle(m_pos, m_radius); }

	void explode();
	bool isExploding() const { return m_exploding; }

private:
	float m_radius;

	bool m_exploding = false;
	float explodeTimer = 0.0f;

};
