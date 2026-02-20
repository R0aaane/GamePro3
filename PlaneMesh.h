
#pragma once

#include "Mesh.h"

class PlaneMesh : public Mesh
{
public:
	PlaneMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex,
		const XMFLOAT2& uvScale = Ones2d, 
		const XMFLOAT2& uvPos = ZeroVec2d,
		bool translucent = false, bool tangentFlag = false);
	virtual ~PlaneMesh();

	void draw() override;

};







