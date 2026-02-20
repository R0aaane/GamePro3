

#pragma once

#include "Actor3d.h"
#include "Mesh.h"
#include <memory>

class MeshActor : public Actor3d
{
public:
	MeshActor(class Scene* scene,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d);
	virtual ~MeshActor();

	bool isEnabled();

	void setCurrentMaterial(const MeshMaterial& m);
	void setOriginalMaterial(const MeshMaterial& m);
	void resetMaterial();

	int getInstanceNum() const;
	void setInstanceNum(int ins);

	void setShadowMatrix(Plane plane, XMFLOAT3 light);

protected:
	std::unique_ptr<Mesh>  m_mesh;
	std::vector<int>       m_modelIndex;
	std::vector<ImageData> m_imageData;

};


