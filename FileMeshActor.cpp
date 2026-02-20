
#include "FileMeshActor.h"
#include "FileMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

FileMeshActor::FileMeshActor(Scene* scene, int shaderIndex, const wchar_t* filePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale,
	const XMFLOAT2& uvScale, const XMFLOAT2& uvPos, bool translucent,
	int shaderIndexFront, float depthOffset)
	: MeshActor(scene, pos, vel, angle, avel, scale)
{
	Renderer* renderer = m_scene->getGame()->getRenderer();

	std::vector<BufferData> data;
	if (!renderer->loadGmesh(filePath, data)) return;

	m_modelIndex.resize(data.size());
	m_imageData.resize(data.size());
	for (size_t i = 0; i < data.size(); ++i)
	{
		MeshConstData mesh;
		m_modelIndex[i] = renderer->allocateConstBuffer((void*)&mesh, sizeof(MeshConstData));
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

	m_mesh = std::make_unique<FileMesh>(this, renderer, shaderIndex, m_modelIndex,
		data, uvScale, uvPos, translucent, shaderIndexFront, depthOffset);
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

FileMeshActor::~FileMeshActor()
{
}

void FileMeshActor::update(float deltaTime)
{
	simulate(deltaTime);
}

void FileMeshActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

