
#pragma once

#include "MeshActor.h"

class BillboardActor : public MeshActor
{
public:
	BillboardActor(class Scene* scene, int shaderIndex, const wchar_t* texturePath,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		float angle = 0.0f, float avel = 0.0f,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false,
		bool yrot = false);
	virtual ~BillboardActor();

	void update(float deltaTime) override;
	void draw() override;

};





