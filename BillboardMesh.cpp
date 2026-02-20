
#include "BillboardMesh.h"
#include "Actor3d.h"
#include "Renderer.h"

BillboardMesh::BillboardMesh(Actor3d* owner, Renderer* renderer,
	int shaderIndex, const std::vector<int>& modelIndex,
	const XMFLOAT2& uvScale, const XMFLOAT2& uvPos,
	bool translucent, bool yrot)
	: PlaneMesh(owner, renderer, shaderIndex, modelIndex,
		uvScale, uvPos, translucent)
	, m_yrotate(yrot)
{
}

BillboardMesh::~BillboardMesh()
{
}

void BillboardMesh::draw()
{
	MeshConstData data;
	XMMATRIX billMat = (m_yrotate) ?
		calcBillboardMatrixYaxis(m_renderer->getViewMatrix(),
			m_owner->getPos(), m_renderer->getCameraPos())
		: calcBillboardMatrix(m_renderer->getViewMatrix(), m_owner->getPos());
	data.world = m_owner->getWorldMatrix() * billMat;
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

