

#include "StarActor.h"
#include "Scene.h"

StarActor::StarActor(Scene* scene, int shaderIndex,
	const wchar_t* texturePath, bool outer,
	float length, float angle, float angleSpeed,
	const XMFLOAT3& scale, const XMFLOAT2& uvScale,
	const XMFLOAT2& uvPos, bool translucent,
	int shaderIndexFront, float depthOffset)
	: SphereActor(scene, shaderIndex, texturePath, outer, ZeroVec3d, ZeroVec3d,
		ZeroVec3d, ZeroVec3d, scale, uvScale, uvPos, translucent, 
		shaderIndexFront, depthOffset)
	, m_length(length)
	, m_angle(angle)
	, m_angleSpeed(angleSpeed)
{
	float theta = XMConvertToRadians(m_angle);
	setPos(XMFLOAT3(m_length * XMScalarCos(theta), m_length * XMScalarSin(theta), 0.0f));
}

StarActor::~StarActor()
{
}

void StarActor::updateNode(float deltaTime, const XMMATRIX& parentMat)
{
	m_angle += deltaTime * m_angleSpeed;
	m_angle = (m_angle >= 360.0f) ? m_angle - 360.0f : m_angle;
	m_angle = (m_angle < 0.0f) ? m_angle + 360.0f : m_angle;
	float theta = XMConvertToRadians(m_angle);
	setPos(XMFLOAT3(m_length * XMScalarCos(theta), m_length * XMScalarSin(theta), 0.0f));

	calcWorldTransform();
	m_worldTransform *= parentMat;
}








