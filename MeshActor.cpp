
#include "MeshActor.h"
#include "Scene.h"
#include "Game.h"
#include "Renderer.h"

MeshActor::MeshActor(Scene* scene,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale)
	: Actor3d(scene, pos, vel, angle, avel, scale)
{
}

MeshActor::~MeshActor()
{
	for (size_t i = 0; i < m_modelIndex.size(); ++i)
	{
		m_scene->getGame()->getRenderer()->releaseConstBuffer(m_modelIndex[i]);
	}
}

bool MeshActor::isEnabled()
{
	if (m_mesh.get() == nullptr) return false;

	return m_mesh->isEnabled();
}

void MeshActor::setCurrentMaterial(const MeshMaterial& m)
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->setCurrentMaterial(m);
}

void MeshActor::setOriginalMaterial(const MeshMaterial& m)
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->setOriginalMaterial(m);
}

void MeshActor::resetMaterial()
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->resetMaterial();
}

int MeshActor::getInstanceNum() const
{
	if (m_mesh.get() == nullptr) return 1;

	return m_mesh->getInstanceNum();
}

void MeshActor::setInstanceNum(int ins)
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->setInstanceNum(ins);
}

void MeshActor::setShadowMatrix(Plane plane, XMFLOAT3 light)
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->setShadowMatrix(plane, light);
}

