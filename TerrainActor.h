
#pragma once

//#include "PlaneActor.h"
#include "GridMeshActor.h"

class TerrainActor : public GridMeshActor
{
public:
	TerrainActor(class Scene* scene, int shaderIndex, const wchar_t* texturePath,
		const wchar_t* meshName, const GridHeightMap& map,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d, const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d, bool translucent = false);
	~TerrainActor();

	Plane getBoundingPlane() { return Plane(getUp(), m_pos); }
	const GridHeightMap& getBoundingGridHeightMap() { return m_map; }

};
























