

#pragma once

#include "MeshActor.h"

class FileMeshActor : public MeshActor
{
public:
	FileMeshActor(class Scene* scene, int shaderIndex, const wchar_t* filePath,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false,
		int shaderIndexFront = -1, float depthOffset = 0.0f);
	virtual ~FileMeshActor();

	void update(float deltaTime) override;
	void draw() override;

};




