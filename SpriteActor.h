
#pragma once

#include "Actor2d.h"

class SpriteActor : public Actor2d
{
public:
	SpriteActor(class Scene* scene, const std::wstring& filePath,
		int shaderIndex = Renderer::Shader2DAlphaLoopPoint,
		const XMFLOAT2& pos = ZeroVec2d, const XMFLOAT2& vel = ZeroVec2d,
		const XMFLOAT2& scale = Ones2d, float angle = 0.0f, float angleVel = 0.0f,
		const XMFLOAT2& uvPos = ZeroVec2d, const XMFLOAT2& uvVel = ZeroVec2d,
		XMFLOAT2* spriteSize = nullptr, XMFLOAT2* uvSize = nullptr,
		bool centerFlag = true, bool ddsFlag = false);
	virtual ~SpriteActor();

	void setScale(XMFLOAT2 s) override;

	virtual void update(float deltaTime) override;
	virtual void draw() override;

	bool isEnabled() const { return m_enabled; }
	void setColorVector(XMFLOAT3 color, float alpha);
	XMFLOAT4 getColorVector() const;

protected:
	bool m_enabled;
	XMFLOAT2 m_offset;
	ImageData m_imgData;
	XMFLOAT2 m_uvPos;
	XMFLOAT2 m_uvVel;
	XMFLOAT2 m_spriteSize;
	XMFLOAT2 m_uvSize;
	bool m_center;
	int m_shaderIndex;
	int m_modelIndex;
	XMFLOAT3 m_color;
	float    m_alpha;

};




