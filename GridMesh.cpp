
#include "GridMesh.h"
#include "Actor3d.h"
#include "Renderer.h"

GridMesh::GridMesh(Actor3d* owner, class Renderer* renderer,
	int shaderIndex, const std::vector<int>& modelIndex,
	const wchar_t* meshName, const GridHeightMap& map,
	const XMFLOAT2& uvScale,
	const XMFLOAT2& uvPos,
	bool translucent)
	: Mesh(owner, renderer, shaderIndex, modelIndex,
		uvScale, uvPos, translucent)
{
	m_meshData.resize(1);
	m_meshData[0] = m_renderer->createGridMesh(meshName, map);
	if (m_meshData[0].indexNum == -1)
	{
		m_meshData.clear();
		return;
	}

	m_enabled = true;
}

GridMesh::~GridMesh()
{
}

void GridMesh::draw()
{
	MeshConstData data;
	data.world = m_owner->getWorldMatrix();
	data.uvTrans = calcMeshUVMatrix(m_uvScale, m_uvPos);
	data.scale = m_owner->getScale();
	data.material = m_curMaterial;
	data.shadow = m_shadow;
	m_renderer->uploadConstBuffer(m_modelIndex[0],
		(void*)&data, sizeof(MeshConstData));
	if (m_translucent)
	{
		m_renderer->drawMeshTranslucent(m_owner->getPos(),
			m_shaderIndex, m_modelIndex[0],
			m_meshData[0], m_meshData[0].indexNum, 0, m_instanceNum);
	}
	else
	{
		m_renderer->drawMesh(m_shaderIndex, m_modelIndex[0],
			m_meshData[0], m_meshData[0].indexNum, 0, m_instanceNum);
	}
}

