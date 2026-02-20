
#pragma once

#include "Mesh.h"

class FileAnimMesh : public Mesh
{
public:
	FileAnimMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex,
		const std::vector<BufferData>& buffer);
	virtual ~FileAnimMesh();

	void draw() override;

	UINT getAnimIndex() const { return m_animIndex; }
	void setAnimIndex(UINT index);
	float getInterval() const { return m_animInterval; }
	unsigned short getFrameNum() const { return m_animFrameNum; }

protected:
	XMMATRIX m_adjust;
	std::vector<AnimData>          m_animData;
	std::vector<AnimMeshConstData> m_animConstData;
	std::vector<std::vector<std::vector<XMMATRIX>>> m_poseMatrices;
	UINT m_animIndex;
	float m_animInterval;
	unsigned short m_animFrameNum;

};





