
#include "FileAnimMesh.h"
#include "Actor3d.h"
#include "Renderer.h"

FileAnimMesh::FileAnimMesh(Actor3d* owner, Renderer* renderer,
	int shaderIndex, const std::vector<int>& modelIndex,
	const std::vector<BufferData>& buffer)
	: Mesh(owner, renderer, shaderIndex, modelIndex, Ones2d, ZeroVec2d)
	, m_adjust(buffer[0].adjust)
	, m_animIndex(0)
{
	m_meshData.resize(buffer.size());
	m_animData.resize(buffer.size());
	m_animConstData.resize(buffer.size());
	m_poseMatrices.resize(buffer.size());

	for (size_t i = 0; i < buffer.size(); ++i)
	{
		m_meshData[i] = buffer[i].mesh;
		m_animData[i] = buffer[i].animData;

		memcpy((void*)m_animConstData[i].inverseMatrices,
			(void*)buffer[i].inverseGPoseMatrices.data(),
			m_animData[i].boneNum * sizeof(XMMATRIX));
		memcpy((void*)m_animConstData[i].poseMatrices,
			(void*)buffer[i].poseMatrices[0].data(),
			m_animData[i].boneNum * sizeof(XMMATRIX));
		m_poseMatrices[i] = buffer[i].poseMatrices;
	}
	m_animInterval = m_animData[0].interval;
	m_animFrameNum = m_animData[0].frameNum;

	m_enabled = true;
}

FileAnimMesh::~FileAnimMesh()
{
}

void FileAnimMesh::draw()
{
	for (size_t i = 0; i < m_meshData.size(); ++i)
	{
		m_animConstData[i].setWorld(m_adjust * m_owner->getWorldMatrix());
		m_animConstData[i].setMaterial(m_curMaterial);
		m_animConstData[i].scale = m_owner->getScale();
		m_animConstData[i].shadow = m_shadow;
		m_renderer->uploadConstBuffer(m_modelIndex[i],
			(void*)&m_animConstData[i], sizeof(AnimMeshConstData));
		m_renderer->drawMesh(m_shaderIndex, m_modelIndex[i],
			m_meshData[i], m_meshData[i].indexNum, 0, m_instanceNum);
	}
}

void FileAnimMesh::setAnimIndex(UINT index)
{
	if (index < 0 || index >= m_animFrameNum) return;
	if (m_animIndex == index) return;

	for (size_t i = 0; i < m_animData.size(); ++i)
	{
		m_animConstData[i].setPoseMatrices(m_poseMatrices[i][index]);
	}
	m_animIndex = index;
}
