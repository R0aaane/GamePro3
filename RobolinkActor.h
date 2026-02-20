
#pragma once

#include "CubeActor.h"

class RobolinkActor : public CubeActor
{
public:
	RobolinkActor(class Scene* scene, int shaderIndex,
		std::vector<const wchar_t*> texturePath, bool outer,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false,
		int shaderIndexFront = -1, float depthOffset = 0.0f);
	~RobolinkActor();

	void updateNode(float deltaTime, const XMMATRIX& parentMat) override;

};




