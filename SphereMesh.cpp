
#include "SphereMesh.h"
#include "Actor3d.h"
#include "Renderer.h"

SphereMesh::SphereMesh(Actor3d* owner, Renderer* renderer,
	int shaderIndex, const std::vector<int>& modelIndex, bool outer,
	const XMFLOAT2& uvScale, const XMFLOAT2& uvPos, bool translucent, 
	int shaderIndexFront, float depthOffset)
	: Mesh(owner, renderer, shaderIndex, modelIndex, uvScale, uvPos,
		translucent, shaderIndexFront, depthOffset)
{
	m_meshData.resize(1);
	m_meshData[0] = (outer) ?
		m_renderer->getMeshData(Renderer::SphereOuterMesh)
		: m_renderer->getMeshData(Renderer::SphereInnerMesh);
	if (m_meshData[0].indexNum == -1)
	{
		m_meshData.clear();
		return;
	}

	m_enabled = true;
}

SphereMesh::~SphereMesh()
{
}

void SphereMesh::draw()
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
		if (m_shaderIndexFront >= 0)
		{
			m_renderer->drawMeshTranslucent(
				m_owner->getPos() + m_depthOffset * m_renderer->getDepthVector(),
				m_shaderIndexFront, m_modelIndex[0],
				m_meshData[0], m_meshData[0].indexNum, 0, m_instanceNum);
		}
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

