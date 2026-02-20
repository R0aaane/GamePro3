
#include "CubeActor.h"
#include "CubeMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

CubeActor::CubeActor(Scene* scene, int shaderIndex,
	std::vector<const wchar_t*> texturePath, bool outer,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel,
	const XMFLOAT3& scale, const XMFLOAT2& uvScale,
	const XMFLOAT2& uvPos, bool translucent, int shaderIndexFront,
	float depthOffset)
	: MeshActor(scene, pos, vel, angle, avel, scale)
{
	Renderer* renderer = m_scene->getGame()->getRenderer();

	if (!(texturePath.size() == 1 || texturePath.size() == 6)) return;

	m_modelIndex.resize(texturePath.size());
	m_imageData.resize(texturePath.size());
	for (size_t i = 0; i < m_modelIndex.size(); ++i)
	{
		MeshConstData data;
		m_modelIndex[i] = renderer->allocateConstBuffer((void*)&data,
			sizeof(MeshConstData));
		if (m_modelIndex[i] == -1)
		{
			for (int j = (int)i - 1; j > 0; --j)
			{
				renderer->releaseConstBuffer(m_modelIndex[j]);
			}
			m_modelIndex.clear();
			return;
		}

		m_imageData[i] = renderer->allocateShaderResource(texturePath[i]);
		if (m_imageData[i].imgIndex == -1)
		{
			for (int j = (int)i - 1; j > 0; --j)
			{
				renderer->releaseConstBuffer(m_modelIndex[j]);
			}
			m_modelIndex.clear();
			return;
		}
		renderer->setMaterialSlot(m_modelIndex[i], 0, m_imageData[i]);
	}

	m_mesh = std::make_unique<CubeMesh>(this, renderer, shaderIndex, m_modelIndex,
		outer, uvScale, uvPos, translucent, shaderIndexFront, depthOffset);
	if (!m_mesh->isEnabled())
	{
		for (size_t i = 0; i < m_modelIndex.size(); ++i)
		{
			renderer->releaseConstBuffer(m_modelIndex[i]);
		}
		m_modelIndex.clear();
		return;
	}
}

CubeActor::~CubeActor()
{
}

void CubeActor::update(float deltaTime)
{
	simulate(deltaTime);
}

void CubeActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

