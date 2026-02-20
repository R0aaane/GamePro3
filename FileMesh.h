
#pragma once

#include "Mesh.h"

class FileMesh : public Mesh
{
public:
	FileMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex,
		const std::vector<BufferData>& buffer,
		const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d,
		bool translucent = false, int shaderIndexFront = -1,
		float depthOffset = 0.0f);
	virtual ~FileMesh();

	void draw() override;

protected:
	XMMATRIX m_adjust;

};


