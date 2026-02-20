
#pragma once

#include "MeshActor.h"

class FileAnimMeshActor : public MeshActor
{
public:
	FileAnimMeshActor(class Scene* scene, int shaderIndex, const wchar_t* filePath,
		const XMFLOAT3& pos = ZeroVec3d, const XMFLOAT3& vel = ZeroVec3d,
		const XMFLOAT3& angle = ZeroVec3d, const XMFLOAT3& avel = ZeroVec3d,
		const XMFLOAT3& scale = Ones3d);
	virtual ~FileAnimMeshActor();

	void update(float deltaTime) override;
	void draw() override;

	void updateAnim(float deltaTime);

protected:
	float m_time;

};


