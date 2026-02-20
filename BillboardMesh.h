
#pragma once

#include "PlaneMesh.h"

class BillboardMesh : public PlaneMesh
{
public:
	BillboardMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex,
		const XMFLOAT2& uvScale = Ones2d, const XMFLOAT2& uvPos = ZeroVec2d,
		bool translucent = false, bool yrot = false);
	virtual ~BillboardMesh();

	void draw() override;

protected:
	bool m_yrotate;

};

