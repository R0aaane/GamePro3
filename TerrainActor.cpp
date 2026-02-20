
#include "TerrainActor.h"
#include "Game.h"
#include "Scene.h"

TerrainActor::TerrainActor(Scene* scene, int shaderIndex, const wchar_t* texturePath,
	const wchar_t* meshName, const GridHeightMap& map,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel,
	const XMFLOAT3& scale, const XMFLOAT2& uvScale,
	const XMFLOAT2& uvPos, bool translucent)
	: GridMeshActor(scene, shaderIndex, texturePath, meshName, map, pos, vel, angle, avel,
		scale, uvScale, uvPos, translucent)
{
}

TerrainActor::~TerrainActor()
{
}








