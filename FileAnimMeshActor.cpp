
#include "FileAnimMeshActor.h"
#include "FileAnimMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

FileAnimMeshActor::FileAnimMeshActor(Scene* scene, int shaderIndex, const wchar_t* filePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale)
	: MeshActor(scene, pos, vel, angle, avel, scale)
	, m_time(0.0f)
{
	Renderer* renderer = m_scene->getGame()->getRenderer();

	std::vector<BufferData> data;
	if (!renderer->loadGanim(filePath, data)) return;

	m_modelIndex.resize(data.size());
	m_imageData.resize(data.size());
	std::vector<AnimMeshConstData> cd(1);
	for (size_t i = 0; i < data.size(); ++i)
	{
		m_modelIndex[i] = renderer->allocateConstBuffer((void*)&cd[0], sizeof(AnimMeshConstData));
		if (m_modelIndex[i] == -1)
		{
			for (size_t j = 0; j < i; ++j)
			{
				renderer->releaseConstBuffer(m_modelIndex[j]);
			}
			m_modelIndex.clear();
			m_imageData.clear();
			return;
		}

		m_imageData[i] = data[i].texture;
		renderer->setMaterialSlot(m_modelIndex[i], 0, m_imageData[i]);
	}

	m_mesh = std::make_unique<FileAnimMesh>(this, renderer, shaderIndex, m_modelIndex, data);
	if (!m_mesh->isEnabled())
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			renderer->releaseConstBuffer(m_modelIndex[i]);
		}
		m_modelIndex.clear();
		m_imageData.clear();
		return;
	}
}

FileAnimMeshActor::~FileAnimMeshActor()
{
}

void FileAnimMeshActor::update(float deltaTime)
{
	simulate(deltaTime);
	updateAnim(deltaTime);
}

void FileAnimMeshActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

void FileAnimMeshActor::updateAnim(float deltaTime)
{
	FileAnimMesh* mesh = (FileAnimMesh*)m_mesh.get();

	m_time += deltaTime;
	if (m_time >= mesh->getInterval())
	{
		m_time = 0.0f;
		UINT index = (mesh->getAnimIndex() + 1) % mesh->getFrameNum();
		mesh->setAnimIndex(index);
	}
}
