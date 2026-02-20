
#pragma once

#include "SphereActor.h"

class StarActor : public SphereActor
{
public:
	StarActor(class Scene* scene, int shaderIndex,
		const wchar_t* texturePath, bool outer,
		float length, float angle, float angleSpeed,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false,
		int shaderIndexFront = -1, float depthOffset = 0.0f);
	~StarActor();

	void updateNode(float deltaTime, const XMMATRIX& parentMat) override;
	void setAngleSpeed(float speed) { m_angleSpeed = speed; }

private:
	float m_length;
	float m_angle;
	float m_angleSpeed;

};





