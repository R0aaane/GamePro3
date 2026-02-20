
#include "PlaneActor.h"
#include "PlaneMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

PlaneActor::PlaneActor(Scene* scene, int shaderIndex, const wchar_t* texturePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale,
	const XMFLOAT2& uvScale, const XMFLOAT2& uvPos, bool translucent, bool tangentFlag,
	std::vector<const wchar_t*> additionalTextures)
	: MeshActor(scene, pos, vel, angle, avel, scale)
{
	Renderer* renderer = m_scene->getGame()->getRenderer();

	m_modelIndex.resize(1);
	MeshConstData data;
	m_modelIndex[0] = renderer->allocateConstBuffer((void*)&data, sizeof(MeshConstData));
	if (m_modelIndex[0] == -1) return;

	int addTexNum = (int)additionalTextures.size();
	m_imageData.resize(1 + addTexNum);
	m_imageData[0] = renderer->allocateShaderResource(texturePath);
	if (m_imageData[0].imgIndex == -1)
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
	renderer->setMaterialSlot(m_modelIndex[0], 0, m_imageData[0]);
	for (int i = 0; i < additionalTextures.size(); ++i)
	{
		m_imageData[1 + i] = renderer->allocateShaderResource(additionalTextures[i]);
		if (m_imageData[1 + i].imgIndex == -1)
		{
			renderer->releaseConstBuffer(m_modelIndex[0]);
			m_modelIndex.clear();
			return;
		}
		renderer->setMaterialSlot(m_modelIndex[0], 1 + i, m_imageData[1 + i]);
	}

	m_mesh = std::make_unique<PlaneMesh>(this, renderer, shaderIndex, m_modelIndex,
		uvScale, uvPos, translucent, tangentFlag);
	if (!m_mesh->isEnabled())
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
}

PlaneActor::~PlaneActor()
{
}

void PlaneActor::update(float deltaTime)
{
	simulate(deltaTime);
}

void PlaneActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

