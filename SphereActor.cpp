
#include "SphereActor.h"
#include "SphereMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

SphereActor::SphereActor(Scene* scene, int shaderIndex,
	const wchar_t* texturePath, bool outer,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel,
	const XMFLOAT3& scale, const XMFLOAT2& uvScale, const XMFLOAT2& uvPos,
	bool translucent, int shaderIndexFront,	float depthOffset)
	: MeshActor(scene, pos, vel, angle, avel, scale)
{
	Renderer* renderer = m_scene->getGame()->getRenderer();

	m_modelIndex.resize(1);
	MeshConstData data;
	m_modelIndex[0] = renderer->allocateConstBuffer((void*)&data, sizeof(MeshConstData));
	if (m_modelIndex[0] == -1) return;

	m_imageData.resize(1);
	m_imageData[0] = renderer->allocateShaderResource(texturePath);
	if (m_imageData[0].imgIndex == -1)
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
	renderer->setMaterialSlot(m_modelIndex[0], 0, m_imageData[0]);

	m_mesh = std::make_unique<SphereMesh>(this, renderer, shaderIndex, m_modelIndex,
		outer, uvScale, uvPos, translucent, shaderIndexFront, depthOffset);
	if (!m_mesh->isEnabled())
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
}

SphereActor::~SphereActor()
{
}

void SphereActor::update(float deltaTime)
{
	simulate(deltaTime);
}

void SphereActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

