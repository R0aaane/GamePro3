
#pragma once

#include "Mesh.h"

class CubeMesh : public Mesh
{
public:
	CubeMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex,
		bool outer,	const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d,
		bool translucent = false, int shaderIndexFront = -1,
		float depthOffset = 0.0f);
	virtual ~CubeMesh();

	void draw() override;

protected:
	bool m_texturePerFace;

};

