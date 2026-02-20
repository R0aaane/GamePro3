
#pragma once

#include "MeshActor.h"

class GridMeshActor : public MeshActor
{
public:
	GridMeshActor(class Scene* scene, int shaderIndex,
		const wchar_t* texturePath, const wchar_t* meshName,
		const GridHeightMap& map,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false);
	virtual ~GridMeshActor();

	void update(float deltaTime) override;
	void draw() override;

protected:
	GridHeightMap m_map;

};














