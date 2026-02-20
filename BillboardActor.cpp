
#include "BillboardActor.h"
#include "BillboardMesh.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

BillboardActor::BillboardActor(Scene* scene, int shaderIndex, const wchar_t* texturePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	float angle, float avel, const XMFLOAT3& scale,
	const XMFLOAT2& uvScale, const XMFLOAT2& uvPos, bool translucent, bool yrot)
	: MeshActor(scene, pos, vel, XMFLOAT3(0.0f, 180.0f, -90.0f + angle),
		XMFLOAT3(0.0f, 0.0f, avel), scale)
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

	m_mesh = std::make_unique<BillboardMesh>(this, renderer, shaderIndex, m_modelIndex,
		uvScale, uvPos, translucent, yrot);
	if (!m_mesh->isEnabled())
	{
		renderer->releaseConstBuffer(m_modelIndex[0]);
		m_modelIndex.clear();
		return;
	}
}

BillboardActor::~BillboardActor()
{
}

void BillboardActor::update(float deltaTime)
{
	simulate(deltaTime);
}

void BillboardActor::draw()
{
	if (!isEnabled()) return;

	m_mesh->draw();
}

