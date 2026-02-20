
#include "GridMeshActor.h"
#include "GridMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

GridMeshActor::GridMeshActor(Scene* scene, int shaderIndex,
	const wchar_t* texturePath, const wchar_t* meshName,
	const GridHeightMap& map,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel,
	const XMFLOAT3& scale, const XMFLOAT2& uvScale,
	const XMFLOAT2& uvPos, bool translucent)
	: MeshActor(scene, pos, vel, angle, avel, scale)
	, m_map(map)
{
	Renderer* renderer = m_scene->getGame()->getRenderer();

	m_modelIndex.resize(1);
	MeshConstData data;
	m_modelIndex[0] = renderer->allocateConstBuffer((void*)&data, sizeof(MeshConstData));
	if (m_modelIndex[0] == -1) return;

	m_imageData.resize(1);
	m_imageData[0] = renderer->allocateShaderResource(texturePath);
	if (m_imageData[0].imgIndex == -1)
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
	renderer->setMaterialSlot(m_modelIndex[0], 0, m_imageData[0]);

	m_mesh = std::make_unique<GridMesh>(this, renderer, shaderIndex, m_modelIndex,
		meshName, map, uvScale, uvPos, translucent);
	if (!m_mesh->isEnabled())
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
}

GridMeshActor::~GridMeshActor()
{
}

void GridMeshActor::update(float deltaTime)
{
	simulate(deltaTime);
}

void GridMeshActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

