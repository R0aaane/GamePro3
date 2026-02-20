
#include "FileMesh.h"
#include "Actor3d.h"
#include "Renderer.h"

FileMesh::FileMesh(Actor3d* owner, Renderer* renderer,
	int shaderIndex, const std::vector<int>& modelIndex,
	const std::vector<BufferData>& buffer,
	const XMFLOAT2& uvScale, const XMFLOAT2& uvPos,
	bool translucent, int shaderIndexFront,	float depthOffset)
	: Mesh(owner, renderer, shaderIndex, modelIndex,
		uvScale, uvPos, translucent, shaderIndexFront, depthOffset)
	, m_adjust(buffer[0].adjust)
{
	m_meshData.resize(buffer.size());
	for (size_t i = 0; i < buffer.size(); ++i)
	{
		m_meshData[i] = buffer[i].mesh;
	}

	m_enabled = true;
}

FileMesh::~FileMesh()
{
}

void FileMesh::draw()
{
	MeshConstData data;
	data.world = m_adjust * m_owner->getWorldMatrix();
	data.uvTrans = calcMeshUVMatrix(m_uvScale, m_uvPos);
	data.scale = m_owner->getScale();
	data.material = m_curMaterial;
	data.shadow = m_shadow;
	for (size_t i = 0; i < m_meshData.size(); ++i)
	{
		m_renderer->uploadConstBuffer(m_modelIndex[i],
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
			m_renderer->drawMeshTranslucent(m_owner->getPos(), m_shaderIndex,
				m_modelIndex[i], m_meshData[i], m_meshData[i].indexNum, 0,
				m_instanceNum);
		}
		else
		{
			m_renderer->drawMesh(m_shaderIndex, m_modelIndex[i],
				m_meshData[i], m_meshData[i].indexNum, 0, m_instanceNum);
		}
	}
}

