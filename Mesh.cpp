
#include "Mesh.h"
#include "Actor3d.h"
#include "Renderer.h"

Mesh::Mesh(Actor3d* owner, Renderer* renderer,
	int shaderIndex, const std::vector<int>modelIndex,
	XMFLOAT2 uvScale, XMFLOAT2 uvPos, bool translucent,
	int shaderIndexFront, float depthOffset)
	: m_owner(owner)
	, m_renderer(renderer)
	, m_shaderIndex(shaderIndex)
	, m_modelIndex(modelIndex)
	, m_enabled(false)
	, m_uvScale(uvScale)
	, m_uvPos(uvPos)
	, m_translucent(translucent)
	, m_shaderIndexFront(shaderIndexFront)
	, m_depthOffset(depthOffset)
	, m_instanceNum(1)
	, m_shadow(XMMatrixIdentity())
{
}

Mesh::~Mesh()
{
}

void Mesh::setCurrentMaterial(const MeshMaterial& m)
{
	m_curMaterial = m;
}

void Mesh::setOriginalMaterial(const MeshMaterial& m)
{
	m_curMaterial = m;
	m_oriMaterial = m;
}

void Mesh::resetMaterial()
{
	m_curMaterial = m_oriMaterial;
}

void Mesh::setShadowMatrix(Plane plane, XMFLOAT3 light)
{
	plane.dist;
	XMVECTOR planeVec = XMVectorSet(plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	XMVECTOR lightVec = XMVectorSet(light.x, light.y, light.z, 0.0f);

	m_shadow = XMMatrixShadow(planeVec, lightVec);
}




