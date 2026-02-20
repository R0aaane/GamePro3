
#include "CubeMesh.h"
#include "Actor3d.h"
#include "Renderer.h"

CubeMesh::CubeMesh(Actor3d* owner, Renderer* renderer,
	int shaderIndex, const std::vector<int>& modelIndex,
	bool outer, const XMFLOAT2& uvScale, const XMFLOAT2& uvPos,
	bool translucent, int shaderIndexFront,	float depthOffset)
	: Mesh(owner, renderer, shaderIndex, modelIndex, uvScale, uvPos,
		translucent, shaderIndexFront, depthOffset)
	, m_texturePerFace(false)
{
	if (modelIndex.size() == 6) m_texturePerFace = true;

	m_meshData.resize(1);
	m_meshData[0] = (outer) ?
		m_renderer->getMeshData(Renderer::CubeOuterMesh)
		: m_renderer->getMeshData(Renderer::CubeInnerMesh);
	if (m_meshData[0].indexNum == -1)
	{
		m_meshData.clear();
		return;
	}

	m_enabled = true;
}

CubeMesh::~CubeMesh()
{
}

void CubeMesh::draw()
{
	MeshConstData data;
	data.world = m_owner->getWorldMatrix();
	data.uvTrans = calcMeshUVMatrix(m_uvScale, m_uvPos);
	data.scale = m_owner->getScale();
	data.material = m_curMaterial;
	data.shadow = m_shadow;
	if (m_texturePerFace)
	{
		XMFLOAT3 s = m_owner->getScale();
		XMFLOAT3 p = m_owner->getPos();
		XMFLOAT3 forward = m_owner->getForward();
		XMFLOAT3 right = m_owner->getRight();
		XMFLOAT3 up = m_owner->getUp();
		XMFLOAT3 pos[6];
		pos[0] = p + forward * 0.5f * s.x;
		pos[1] = p + right * 0.5f * s.y;
		pos[2] = p - forward * 0.5f * s.x;
		pos[3] = p - right * 0.5f * s.y;
		pos[4] = p + up * 0.5f * s.z;
		pos[5] = p - up * 0.5f * s.z;
		for (int i = 0; i < 6; ++i)
		{
			m_renderer->uploadConstBuffer(m_modelIndex[i],
				(void*)&data, sizeof(MeshConstData));
			if (m_translucent)
			{
				m_renderer->drawMeshTranslucent(pos[i],
					m_shaderIndex, m_modelIndex[i], m_meshData[0], 6, 6 * i,
					m_instanceNum);
			}
			else
			{
				m_renderer->drawMesh(m_shaderIndex, m_modelIndex[i],
					m_meshData[0], 6, 6 * i, m_instanceNum);
			}
		}
	}
	else
	{
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
}

