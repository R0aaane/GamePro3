
#pragma once

#include "Mesh.h"

class HemisphereMesh : public Mesh
{
public:
	HemisphereMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex, bool outer,
		const XMFLOAT2& uvScale = Ones2d, const XMFLOAT2& uvPos = ZeroVec2d,
		bool translucent = false, int shaderIndexFront = -1,
		float depthOffset = 0.0f);
	virtual ~HemisphereMesh();

	void draw() override;

};










