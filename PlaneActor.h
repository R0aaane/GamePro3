
#pragma once

#include "MeshActor.h"

class PlaneActor : public MeshActor
{
public:
	PlaneActor(class Scene* scene, int shaderIndex, const wchar_t* texturePath,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false,
		bool tangentFlag = false, std::vector<const wchar_t*> additionalTextures = {});
	virtual ~PlaneActor();

	void update(float deltaTime) override;
	void draw() override;

};




