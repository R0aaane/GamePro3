
#include "RobolinkActor.h"
#include "Scene.h"

RobolinkActor::RobolinkActor(Scene* scene, int shaderIndex,
	std::vector<const wchar_t*> texturePath, bool outer,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel,
	const XMFLOAT3& scale, const XMFLOAT2& uvScale,
	const XMFLOAT2& uvPos, bool translucent,
	int shaderIndexFront, float depthOffset)
	: CubeActor(scene, shaderIndex, texturePath, outer, pos, vel,
		angle, avel, scale, uvScale, uvPos, translucent, 
		shaderIndexFront, depthOffset)
{
}

RobolinkActor::~RobolinkActor()
{
}

void RobolinkActor::updateNode(float deltaTime, const XMMATRIX& parentMat)
{
	simulate(deltaTime);
	calcWorldTransform();

	m_worldTransform *= parentMat;
}




