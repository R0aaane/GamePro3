#pragma once

#include "BlockAnimActor.h"

class PlayerActor2D : public BlockAnimActor
{
public:
	PlayerActor2D(class Scene* scene, const std::wstring& filePath,
		const std::vector<std::vector<UINT>>& indices,
		UINT lane, float interval, UINT wNum = 3, UINT hNum = 4,
		int shaderIndex = Renderer::Shader2DAlphaLoopPoint,
		const XMFLOAT2& pos = ZeroVec2d, const XMFLOAT2& vel = ZeroVec2d,
		const XMFLOAT2& scale = Ones2d, float angle = 0.0f, float angleVel = 0.0f,
		XMFLOAT2* spriteSize = nullptr, bool centerFlag = true, bool ddsFlag = false);
	virtual ~PlayerActor2D();

	void update(float deltaTime) override;
	void draw() override;

private:

};
